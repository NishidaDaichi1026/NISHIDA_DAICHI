#include	"CPlaneMesh.h"
#include	<iostream>

void CPlaneMesh::Init(
	int divx, int divy,
	int width, int height,
	DirectX::SimpleMath::Color color,
	DirectX::SimpleMath::Vector3 normal,
	bool xzflag) 
{
	// サイズセット（幅と高さ）（XY平面）
	m_width = static_cast<float>(width);
	m_height = static_cast<float>(height);

	// 分割数
	m_divY = divy;
	m_divX = divx;

	// 法線ベクトル
	m_normal = normal;

	// 頂点カラー
	m_color = color;

	if (xzflag == true) {
		// 頂点データ生成
		CreateVertexXZ();
	}
	else {
		// 頂点データ生成
		CreateVertex();
	}
	// インデックスデータ生成
	CreateIndex();

}

void CPlaneMesh::CreateVertex() {
	// 頂点データクリア
	m_vertices.clear();

	for (unsigned int y = 0; y <= m_divY; y++) {
		VERTEX_3D	vtx{};

		for (unsigned int x = 0; x <= m_divX; x++) {

			// 頂点座標セット
			vtx.Position.x = -m_width / 2.0f + x * m_width / m_divX;
			vtx.Position.y = -m_height / 2.0f + y * m_height / m_divY;
			vtx.Position.z = 0.0f;

			// 法線ベクトルセット
			vtx.Normal = m_normal;				// 法線をセット

			vtx.Diffuse = m_color;					// カラー値セット

			// 繰り返しに対応（テクスチャの）
			float texu = 1.0f * m_divX;
			float texv = 1.0f * m_divY;
			vtx.TexCoord.x = (texu * x / m_divX);
			vtx.TexCoord.y = (texv * y / m_divY);

			m_vertices.emplace_back(vtx);		// 頂点データセット
		}
	}
}

void CPlaneMesh::CreateVertexXZ() {
	// 頂点データクリア
	m_vertices.clear();

	for (unsigned int y = 0; y <= m_divY; y++) {
		VERTEX_3D	vtx{};

		for (unsigned int x = 0; x <= m_divX; x++) {

			// 頂点座標セット
			vtx.Position.x = -m_width / 2.0f + x * m_width / m_divX;
			vtx.Position.y = 0.0f;
			vtx.Position.z = -m_height / 2.0f + y * m_height / m_divY;

			// 法線ベクトルセット
			vtx.Normal = m_normal;				// 法線をセット

			vtx.Diffuse = m_color;					// カラー値セット

			// 繰り返しに対応（テクスチャの）
			float texu = 1.0f * m_divX;
			float texv = 1.0f * m_divY;
			vtx.TexCoord.x = (texu * x / m_divX);
			vtx.TexCoord.y = (texv * y / m_divY);

			m_vertices.emplace_back(vtx);		// 頂点データセット
		}
	}
}

void CPlaneMesh::CreateIndex() {
	// インデックスデータクリア
	m_indices.clear();

	// インデックス生成
	for (unsigned int y = 0; y < m_divY; y++) {
		for (unsigned int x = 0; x < m_divX; x++) {
			int count = (m_divX + 1) * y + x;		// 左下座標のインデックス

			// 下半分
			{
				FACE face{};

				face.idx[0] = count;						// 左下
				face.idx[1] = count + 1 + (m_divX + 1);		// 右上
				face.idx[2] = count + 1;					// 右

				m_indices.emplace_back(face.idx[0]);
				m_indices.emplace_back(face.idx[1]);
				m_indices.emplace_back(face.idx[2]);

			}

			// 上半分
			{
				FACE face{};
				face.idx[0] = count;						// 左下
				face.idx[1] = count + (m_divX + 1);			// 上
				face.idx[2] = count + (m_divX + 1) + 1;		// 右上

				m_indices.emplace_back(face.idx[0]);
				m_indices.emplace_back(face.idx[1]);
				m_indices.emplace_back(face.idx[2]);

			}
		}
	}
}

// 幅
float CPlaneMesh::GetWidth() {
	return m_width;
}

// 高さ
float CPlaneMesh::GetHeight() {
	return m_height;
}

int CPlaneMesh::GetDivX() {
	return m_divX;
}

int CPlaneMesh::GetDivY() {
	return m_divY;
}

	// 指定した3角形番号の三角形インデックスを取得
CPlaneMesh::FACE CPlaneMesh::GetTriangle(int triangleno) {
	FACE face;
	face.idx[0] = m_indices[triangleno * 3];
	face.idx[1] = m_indices[triangleno * 3 + 1];
	face.idx[2] = m_indices[triangleno * 3 + 2];
	return face;
}


// 何番目の四角形かを見つける
int CPlaneMesh::GetSquareNo(DirectX::SimpleMath::Vector3 pos)
{
	// 床メッシュはXZ平面をベースで作成している
	double x = pos.x;
	double y = pos.z;

	// 平面の幅
	double planewidth = m_width;

	// 平面の高さ
	double planeheight = m_height;

	// マップチップサイズ（絶対値で計算）
	double mapchipwidth = fabs(planewidth / m_divX);
	double mapchipheight = fabs(planeheight / m_divY);

	// 左下原点の相対座標に変換
	double relativex = x + (planewidth / 2.0);
	double relativey = y + (planeheight / 2.0);

	// 左から何番目か？
	unsigned int mapchipx = static_cast<unsigned int>(relativex / mapchipwidth);

	// 下から何番目か？
	unsigned int mapchipy = static_cast<unsigned int>(relativey / mapchipheight);

	// 左下を０番目とした場合の順番を計算
	int squareno;
	squareno = mapchipy * m_divX + mapchipx;

	if (squareno < 0) {
		squareno = 0;
	}
	else {
		if (squareno > m_divX * m_divY - 1) {
			squareno = m_divX * m_divY - 1;
		}
	}

	//
//	std::cout << "mapchipx mapchipy: " << mapchipx << "," << mapchipy << "," << squareno << "(" << x << "," << y << ")" << std::endl;

	return squareno;
}


void CPlaneMeshWithoutCommon::Init(int divx, int divy,
	int width, int height,
	DirectX::SimpleMath::Color color,
	DirectX::SimpleMath::Vector3 normal) {

	// サイズセット（幅と高さ）（XY平面）
	m_width = static_cast<float>(width);
	m_height = static_cast<float>(height);

	// 分割数
	m_divY = divy;
	m_divX = divx;

	// 法線ベクトル
	m_normal = normal;

	// 頂点カラー
	m_color = color;

	// インデックスと頂点データを生成
	CreateIndex();
}

void CPlaneMeshWithoutCommon::CreateIndex() {
	struct FACE {
		unsigned int idx[3];
	};

	// インデックスデータクリア
	m_indices.clear();
	m_vertices.clear();

	unsigned int quadnum = 0;

	for (int y = 0; y < m_divY; y++) {
		for (int x = 0; x < m_divX; x++) {

			VERTEX_3D v;

			v.Position.x = -m_width / 2.0f + x * m_width / m_divX;
			v.Position.y = -m_height / 2.0f + y * m_height / m_divY;
			v.Position.z = 0.0f;
			v.TexCoord.x = 0.0f;
			v.TexCoord.y = 1.0f;

			v.Normal = m_normal;

			v.Diffuse = m_color;
			m_vertices.emplace_back(v);

			v.Position.x = -m_width / 2.0f + (x + 1) * m_width / m_divX;
			v.Position.y = -m_height / 2.0f + y * m_height / m_divY;
			v.Position.z = 0.0f;
			v.TexCoord.x = 1.0f;
			v.TexCoord.y = 1.0f;
			v.Normal = m_normal;
			v.Diffuse = m_color;
			m_vertices.emplace_back(v);

			v.Position.x = -m_width / 2.0f + x * m_width / m_divX;
			v.Position.y = -m_height / 2.0f + (y + 1) * m_height / m_divY;
			v.Position.z = 0.0f;
			v.TexCoord.x = 0.0f;
			v.TexCoord.y = 0.0f;
			v.Normal = m_normal;
			v.Diffuse = m_color;
			m_vertices.emplace_back(v);

			v.Position.x = -m_width / 2.0f + (x + 1) * m_width / m_divX;
			v.Position.y = -m_height / 2.0f + (y + 1) * m_height / m_divY;
			v.Position.z = 0.0f;
			v.TexCoord.x = 1.0f;
			v.TexCoord.y = 0.0f;
			v.Diffuse = m_color;
			v.Normal = m_normal;
			m_vertices.emplace_back(v);

			FACE f;

			// インデックスデータ作成
			f.idx[2] = quadnum * 4;
			f.idx[1] = quadnum * 4 + 1;
			f.idx[0] = quadnum * 4 + 2;
			m_indices.emplace_back(f.idx[0]);
			m_indices.emplace_back(f.idx[1]);
			m_indices.emplace_back(f.idx[2]);

			f.idx[2] = quadnum * 4 + 1;
			f.idx[1] = quadnum * 4 + 3;
			f.idx[0] = quadnum * 4 + 2;
			m_indices.emplace_back(f.idx[0]);
			m_indices.emplace_back(f.idx[1]);
			m_indices.emplace_back(f.idx[2]);

			quadnum++;
		}
	}

	DirectX::SimpleMath::Vector3 p0p1;
	DirectX::SimpleMath::Vector3 p0p2;
	DirectX::SimpleMath::Vector3 n;

	p0p1 = m_vertices[1].Position - m_vertices[0].Position;
	p0p2 = m_vertices[2].Position - m_vertices[0].Position;

	// 外積を計算
	p0p1.Cross(p0p1, n);

	for (auto& v : m_vertices) {
		v.Normal = n;
	}
}

// 幅
float CPlaneMeshWithoutCommon::GetWidth() {
	return m_width;
}

// 高さ
float CPlaneMeshWithoutCommon::GetHeight() {
	return m_height;
}

int CPlaneMeshWithoutCommon::GetDivX() {
	return m_divX;
}

int CPlaneMeshWithoutCommon::GetDivY() {
	return m_divY;
}
