#pragma once
#include	"CVertexBuffer.h"
#include	"CIndexBuffer.h"
#include	"CMesh.h"

class CMeshRenderer {
protected:
	CVertexBuffer<VERTEX_3D>	m_VertexBuffer;		// 頂点バッファ
	CIndexBuffer				m_IndexBuffer;		// インデックスバッファ
	int							m_IndexNum = 0;		// インデックス数
public:
	virtual void Init(CMesh& mesh) {
		m_VertexBuffer.Create(mesh.GetVertices());
		m_IndexBuffer.Create(mesh.GetIndices());
		m_IndexNum = static_cast<int>(mesh.GetIndices().size());
	}

	virtual void Draw() {
		ID3D11DeviceContext* devicecontext;

		devicecontext = Renderer::GetDeviceContext();

		// トポロジーをセット（旧プリミティブタイプ）
		devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_VertexBuffer.SetGPU();
		m_IndexBuffer.SetGPU();

		devicecontext->DrawIndexed(
			m_IndexNum,								// 描画するインデックス数（面数×３）
			0,										// 最初のインデックスバッファの位置
			0);										// 頂点バッファの最初から使う
	}
};
