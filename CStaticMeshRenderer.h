#pragma once
#include	<vector>
#include	"renderer.h"
#include	"CStaticMesh.h"
#include	"CShader.h"
#include	"CMeshRenderer.h"

class CStaticMeshRenderer : public CMeshRenderer {
public:
	void Draw() override;
	void Init(CStaticMesh& mesh);
	void Uninit();
private:
	std::vector<ID3D11ShaderResourceView*>	m_DiffuseSRVtable;		// ディフューズテクスチャSRV
	std::vector<SUBSET>	m_Subsets;									// サブセット情報

	std::vector<CMaterial*>		m_Materials;						// マテリアル群
};
