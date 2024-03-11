#include	"CStaticMesh.h"
#include	"renderer.h"

using namespace DirectX;

	// モデルファイルを初期化する
	bool CStaticMesh::Init(
		std::string filename)			//	３Ｄモデルファイル
	{
		bool sts{};

		m_filename = filename;			// テクスチャ読み込み用に保存

		// シーン情報構築
		Assimp::Importer importer;

		// シーン情報を構築
		const aiScene* pScene = importer.ReadFile(
			filename.c_str(),
			aiProcess_ConvertToLeftHanded |
			// 左手座標系に変換する
//		aiProcess_PopulateArmatureData |
//		aiProcess_PreTransformVertices |
		// ノード構造を削除して　それらのノードが持つローカル変換行列ですべての頂点を事前に変換する
			aiProcess_Triangulate);
		// 三角形化する
//		aiProcess_GenSmoothNormals |
//		aiProcess_JoinIdenticalVertices);

		if (pScene == nullptr) {

			printf("load error %s ( %s ) \n ", filename.c_str(), importer.GetErrorString());
			return false;
		}

		/*
			変わらず 表示がおかしい
			const aiScene* pScene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
			if (pScene == nullptr) {
				printf("load error %s ( %s ) \n ", filename.c_str(), importer.GetErrorString());
				return false;
			}
		*/

		// メッシュ数でメッシュコンテナ配列数を決定
		m_subsets.resize(pScene->mNumMeshes);

		// メッシュ数分ループ
		for (unsigned int m = 0; m < pScene->mNumMeshes; m++) {

			aiMesh* mesh{};
			mesh = pScene->mMeshes[m];

			// メッシュ単位で頂点データを取得
			int vnum = GetVertices(mesh);

			// メッシュ単位でインデックスデータ取得
			int idxnum = GetIndices(mesh);

			// メッシュで使用しているマテリアル情報を取得
			int mtrlidx = mesh->mMaterialIndex;

			// インデックス数を保存
			m_subsets[m].IndexNum = idxnum;

			// 頂点数を保存
			m_subsets[m].VertexNum = vnum;

			// マテリアルインデックスを保存
			m_subsets[m].MaterialIdx = mtrlidx;
		}

		// メッシュのベース情報を構築
		CalcMeshBaseIndex();

		// 全テクスチャを取得
		GetTextures(pScene);

		// マテリアルを取得
		GetMaterials(pScene);

		/*
		{
			FILE* fp;
			int i = 0;
			fopen_s(&fp, "vertex.txt", "w");
			for (auto& v : m_vertices) {
				fprintf(fp, "(%d) %f %f %f \n", i, v.Position.x, v.Position.y, v.Position.z);
				i++;
			}
			fclose(fp);
		}

		{
			FILE* fp;
			int i = 0;
			fopen_s(&fp, "index.txt", "w");
			for (auto& index : m_indices) {
				fprintf(fp, "(%d) %d \n", i, index);
				i++;
			}
			fclose(fp);
		}		
		*/

		return true;
	}

	void CStaticMesh::Exit() {

		// SRV解放
	//	for (auto& srv : m_AllTexture) {
	//		srv->Release();
	//	}

		// SRV解放
		for (auto& texres : m_Texturedic) {
			if (texres.second != nullptr) {
				texres.second->Release();
			}
		}
	}

	// 全頂点をaiMeshから取得する
	unsigned int  CStaticMesh::GetVertices(const aiMesh* aimesh) {

		// 頂点数分ループ
		for (unsigned int vidx = 0; vidx < aimesh->mNumVertices; vidx++) {

			VERTEX_3D v;

			v.Position.x = aimesh->mVertices[vidx].x;
			v.Position.y = aimesh->mVertices[vidx].y;
			v.Position.z = aimesh->mVertices[vidx].z;

			// 法線あり？
			if (aimesh->HasNormals()) {
				v.Normal.x = aimesh->mNormals[vidx].x;
				v.Normal.y = aimesh->mNormals[vidx].y;
				v.Normal.z = aimesh->mNormals[vidx].z;
			}
			else {
				v.Normal = XMFLOAT3(0, 0, 0);
			}

			// テクスチャあり？
			if (aimesh->HasTextureCoords(0)) {
				v.TexCoord.x = aimesh->mTextureCoords[0][vidx].x;				// 20230809 bug fix by ts
				v.TexCoord.y = aimesh->mTextureCoords[0][vidx].y;				// 20230809 bug fix by ts
			}
			else {
				v.TexCoord = XMFLOAT2(0, 0);
			}

			// 頂点カラーあり？
			if (aimesh->HasVertexColors(0)) {
				v.Diffuse.x = aimesh->mColors[0][vidx].r;
				v.Diffuse.y = aimesh->mColors[0][vidx].g;
				v.Diffuse.z = aimesh->mColors[0][vidx].b;
				v.Diffuse.w = aimesh->mColors[0][vidx].a;
			}
			else {
				v.Diffuse = XMFLOAT4(1, 1, 1, 1);								// 無かった場合は白
			}

			// 頂点データを保存
			m_vertices.emplace_back(v);
		}

		// aiMeshが保持する頂点数を戻す
		return aimesh->mNumVertices;
	}

	// aiMeshから全インデックス取得
	unsigned int  CStaticMesh::GetIndices(const aiMesh* aimesh) {

		// 面数分ループ
		for (unsigned int f = 0; f < aimesh->mNumFaces; f++)
		{
			const aiFace* face = &aimesh->mFaces[f];
			assert(face->mNumIndices == 3);

			// インデックスデータを保存
			m_indices.emplace_back(face->mIndices[0]);
			m_indices.emplace_back(face->mIndices[1]);
			m_indices.emplace_back(face->mIndices[2]);

		}

		return aimesh->mNumFaces * 3;
	}

	// ベース情報を再計算
	void CStaticMesh::CalcMeshBaseIndex() {

		// サブセット数分ループ
		for (int m = 0; m < m_subsets.size(); m++)
		{
			// 頂点バッファのベースを計算
			m_subsets[m].VertexBase = 0;
			for (int i = m - 1; i >= 0; i--) {
				m_subsets[m].VertexBase += m_subsets[i].VertexNum;
			}

			// インデックスバッファのベースを計算
			m_subsets[m].IndexBase = 0;
			for (int i = m - 1; i >= 0; i--) {
				m_subsets[m].IndexBase += m_subsets[i].IndexNum;
			}
		}
	}

	// すべてのテクスチャSRVを取得
	void CStaticMesh::GetTextures(const aiScene* aiscene) {

		ID3D11DeviceContext* devicecontext;
		ID3D11Device* device;

//		device = CDirectXGraphics::GetInstance()->GetDXDevice();
//		devicecontext = CDirectXGraphics::GetInstance()->GetImmediateContext();

		device = Renderer::GetDevice();
		devicecontext = Renderer::GetDeviceContext();

		// このシーンが持つテクスチャを全て読み込む
		for (unsigned int texidx = 0; texidx < aiscene->mNumTextures; texidx++) {

			// テクスチャを取得
			aiTexture* aitexture = aiscene->mTextures[texidx];

			// 内蔵テクスチャか？
			if (isEmbeddedTexure(aitexture)) {

				ID3D11ShaderResourceView* srv;

				aiTexture* aitexture = aiscene->mTextures[texidx];

				// mHeight is 0, so try to load a compressed texture of mWidth bytes
				const size_t size = aitexture->mWidth;

				// SRV生成
				HRESULT hr = DirectX::CreateWICTextureFromMemory(
					device,
					devicecontext,
					reinterpret_cast<const unsigned char*>(aitexture->pcData),
					aitexture->mWidth,
					nullptr,
					&srv);

				assert(srv);

				// assimpの内部データにセットされているテクスチャ名をキーとして保存
				m_Texturedic[aitexture->mFilename.data] = srv;

			}
			else {
				// 内蔵テクスチャではない場合

				// テクスチャファイル名を取得する
				std::string filename = GetFileName(aitexture->mFilename.C_Str());

				// テクスチャファイルを読み込みSRVを生成
				// SRV
				ID3D11ShaderResourceView* srv = nullptr;
				//			srv = LoadDiffuseTexture(filename);

				bool sts = CreateSRVfromFile(
					filename.c_str(),
					device,
					devicecontext,
					&srv);

				if (sts) {
					// assimpの内部データにセットされているテクスチャ名をキーとして保存
					m_Texturedic[aitexture->mFilename.data] = srv;
				}
				else {
					m_Texturedic[aitexture->mFilename.data] = nullptr;
				}
			}
		}
	}

	// 内蔵テクスチャかどうかを識別する
	bool CStaticMesh::isEmbeddedTexure(const aiTexture* aitexture) {

		if (aitexture->mHeight == 0 ||
			aitexture->mWidth == 0 ||
			aitexture->pcData == nullptr) {
			return true;
		}
		return false;
	}

	// 個々のマテリアル情報（反射情報）取得
	void CStaticMesh::GetaMatreial(const aiMaterial* aimtrl) {

		// デバイス、デバイスコンテキスト取得
		ID3D11DeviceContext* devicecontext;
		ID3D11Device* device;

//		device = CDirectXGraphics::GetInstance()->GetDXDevice();
//		devicecontext = CDirectXGraphics::GetInstance()->GetImmediateContext();


		device = Renderer::GetDevice();
		devicecontext = Renderer::GetDeviceContext();

		// マテリアル情報
		MATERIAL mymaterial{};

		// 拡散反射成分
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if (aimtrl->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
				mymaterial.Diffuse.x = color.r;
				mymaterial.Diffuse.y = color.g;
				mymaterial.Diffuse.z = color.b;
				mymaterial.Diffuse.w = 1.0f;
			}
			else {
				mymaterial.Diffuse.x = 0.5f;
				mymaterial.Diffuse.y = 0.5f;
				mymaterial.Diffuse.z = 0.5f;
				mymaterial.Diffuse.w = 1.0f;
			}
		}

		// 鏡面反射成分
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if (aimtrl->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
				mymaterial.Specular.x = color.r;
				mymaterial.Specular.y = color.g;
				mymaterial.Specular.z = color.b;
				mymaterial.Specular.w = 1.0f;
			}
			else {
				mymaterial.Specular.x = 0.0f;
				mymaterial.Specular.y = 0.0f;
				mymaterial.Specular.z = 0.0f;
				mymaterial.Specular.w = 0.0f;
			}
		}

		// 鏡面反射強度
		{
			float Shinness = 0.0f;

			if (aimtrl->Get(AI_MATKEY_SHININESS, Shinness) == AI_SUCCESS) {
				mymaterial.Shininess = Shinness;
			}
			else {
				mymaterial.Shininess = 0.0f;
			}
		}

		// 環境光反射成分
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if (aimtrl->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
				mymaterial.Ambient.x = color.r;
				mymaterial.Ambient.y = color.g;
				mymaterial.Ambient.z = color.b;
				mymaterial.Ambient.w = 1.0f;
			}
			else {
				mymaterial.Ambient.x = 0.0f;
				mymaterial.Ambient.y = 0.0f;
				mymaterial.Ambient.z = 0.0f;
				mymaterial.Ambient.w = 0.0f;
			}
		}

		// 自家発光成分
		{
			aiColor3D color(0.0f, 0.0f, 0.0f);

			if (aimtrl->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
				mymaterial.Emission.x = color.r;
				mymaterial.Emission.y = color.g;
				mymaterial.Emission.z = color.b;
				mymaterial.Emission.w = 1.0f;
			}
			else {
				mymaterial.Emission.x = 0.0f;
				mymaterial.Emission.y = 0.0f;
				mymaterial.Emission.z = 0.0f;
				mymaterial.Emission.w = 0.0f;
			}
		}

		// ディフューズテクスチャ数取得
		aiTextureType type = aiTextureType_DIFFUSE;
		int texnum = aimtrl->GetTextureCount(type);

		// 1メッシュにディフーズテクスチャ１枚しか許可せず
		assert(texnum <= 1);

		// マテリアル名
		aiString str;

		// assimpからテクスチャファイル名を取得
		aimtrl->GetTexture(type, 0, &str);

		// SRV
		ID3D11ShaderResourceView* srv = nullptr;

		// SRV辞書に存在するか？
		if (m_Texturedic.find(str.data) == m_Texturedic.end()) {
			mymaterial.TextureEnable = FALSE;
			srv = nullptr;

			// pmx の場合シーンに登録されていないテクスチャが存在する
			aimtrl->GetTexture(type, 0, &str);

			// ファイル名を取得する
			std::string filename = GetFileName(str.C_Str());

			// テクスチャを読み込み取得
			// SRV
			srv = LoadDiffuseTexture(filename);

			if (srv == nullptr) {
				mymaterial.TextureEnable = FALSE;
			}
			else {
				mymaterial.TextureEnable = TRUE;
			}
		}
		else {
			mymaterial.TextureEnable = TRUE;
			srv = m_Texturedic[str.data];
		}

		// SRV保存
		m_AllTexture.emplace_back(srv);

		// マテリアル情報を保存
		m_materials.emplace_back(mymaterial);
	}

	// マテリアル取得
	void CStaticMesh::GetMaterials(const aiScene* aiscene) {

		// このモデルで使用しているすべてのマテリアル情報を取得
		for (unsigned int mrlidx = 0; mrlidx < aiscene->mNumMaterials; mrlidx++) {

			// このメッシュ用のマテリアルを取得
			aiMaterial* mtl;
			mtl = aiscene->mMaterials[mrlidx];

			// マテリアル情報取得
			GetaMatreial(mtl);
		}
	}

// diffuse texture load
ID3D11ShaderResourceView* CStaticMesh::LoadDiffuseTexture(std::string filename) {
	// ファイルの拡張子を取得
	std::string fileext = GetFileExt(filename);

	// ファイル名(UTF-16)を取得する
	std::wstring widefilename = GetFileNameWide(filename.c_str());

	// モデルデータの親ディレクトリを取得
	std::wstring parentdirectory = GetParentDirectoryWide(m_filename);// +L"";

	// テクスチャのパス名取得
	std::wstring filepath = parentdirectory + widefilename;

	// SRV
	ID3D11ShaderResourceView* srv = nullptr;

	// TGA？
	if (fileext == ".tga") {
		// TGAの場合
		DirectX::TexMetadata meta;
		DirectX::GetMetadataFromTGAFile(filepath.c_str(), meta);

		std::unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage);

		HRESULT hr = LoadFromTGAFile(filepath.c_str(), &meta, *image);
		if (FAILED(hr)) {
			std::cout << "LoadFromTGAFile Error (" << filename << ")" << std::endl;
			return nullptr;
		}

		ID3D11Resource* tex = nullptr;

		// テクスチャ生成
		hr = CreateTexture(
			Renderer::GetDevice(),
			image->GetImages(),
			image->GetImageCount(),
			meta,
			&tex);

		tex->Release();

		if (FAILED(hr)) {
			std::cout << "CreateTexture Error (" << filename << ")" << std::endl;
			return nullptr;
		}

		// シェーダーリソースビュー作成
		hr = DirectX::CreateShaderResourceView(Renderer::GetDevice(),
			image->GetImages(),
			image->GetImageCount(),
			meta,
			&srv);

		if (FAILED(hr)) {
			std::wcout << L"CreateShaderResourceView Error (" << filepath.c_str() << L")" << std::endl;
			return nullptr;
		}
	}
	else {
		// テクスチャ読み込み
		HRESULT hr = DirectX::CreateWICTextureFromFile(
			Renderer::GetDevice(),
			filepath.c_str(),
			nullptr,
			&srv);
		if (FAILED(hr)) {
			std::wcout << L"CreateWICTextureFromFile Error (" << filepath.c_str() << widefilename.c_str() << L")" << std::endl;
			return nullptr;
		}
	}

	return srv;
}



