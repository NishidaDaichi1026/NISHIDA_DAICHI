#pragma once
#include	<d3d11.h>
#include	<DirectXMath.h>
#include	<wrl/client.h>
#include	<vector>
#include	<string>
#include	<unordered_map>
#include	<WICTextureLoader.h>

// assimp５．２．０用
#include	<assimp\Importer.hpp>
#include	<assimp\scene.h>
#include	<assimp\postprocess.h>
#include	<assimp/cimport.h>

// 頂点定義
#include	"renderer.h"

#include	"dx11helper.h"
#include	"utility.h"
#include	"CMaterial.h"
#include	"CMesh.h"

// assimp 5.2.0 用ライブラリ
//#pragma comment(lib,"assimp-vc142-mtd.lib")

// assimp 5.2.5 用ライブラリ
#pragma comment(lib,"assimp-vc143-mtd.lib")

// COMPTRを使えるように
using Microsoft::WRL::ComPtr;

class CStaticMesh : public CMesh {
private:
	std::vector<SUBSET>			m_subsets;		// サブセット群
	std::vector<MATERIAL>		m_materials;	// マテリアルテーブル

	// メッシュに含まれる頂点を取り出す
	unsigned int GetVertices(const aiMesh* aimesh);
	// メッシュに含まれるインデックスを取り出す
	unsigned int  GetIndices(const aiMesh* aimesh);

	// サブセットを１つにまとめるためのベースを計算する
	void CalcMeshBaseIndex();

	// 全テクスチャを取得(辞書を作成する為)
	void GetTextures(const aiScene* aiscene);

	// 内蔵テクスチャかどうかを識別する
	bool isEmbeddedTexure(const aiTexture* aitexture);

	// assimpの内部データにセットされているテクスチャ名をキーとして保存
	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_Texturedic;

	// マテリアルに含まれるデータを取得する
	void GetMaterials(const aiScene* aiscene);				// 全マテリアル分ループ
	void GetaMatreial(const aiMaterial* aimtrl);			// １つのマテリアル

	// テクスチャリソーステーブル
	std::vector<ID3D11ShaderResourceView*>	m_AllTexture;

	// テクスチャファイルを読み込んでSRVを生成する
	ID3D11ShaderResourceView* LoadDiffuseTexture(std::string filename);

	// このモデルのパス名
	std::string m_filename;								
public:
	// 初期化
	bool Init(std::string filename);

	// 終了処理
	void Exit();

	// 頂点データ取得
	const std::vector<VERTEX_3D>& GetVertices() {
		return m_vertices;
	}

	// インデックスデータ取得
	const std::vector<unsigned int>& GetIndices() {
		return m_indices;
	}

	// サブセットデータ取得
	const std::vector<SUBSET>& GetSubsets() {
		return m_subsets;
	}

	// SRVテーブル取得
	const std::vector<ID3D11ShaderResourceView*>& GetSRVtable() {
		return m_AllTexture;
	}

	// マテリアルデータ取得
	const std::vector<MATERIAL>& GetMaterials() {
		return m_materials;
	}
};

