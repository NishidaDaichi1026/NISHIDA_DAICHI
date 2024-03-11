#pragma once
#include	<simplemath.h>
#include	<random>
#include	"CMesh.h"

class CPlaneMesh : public CMesh {
public:
	struct FACE {
		unsigned int idx[3];
	};

	void Init(int divx, int divy,
		int width, int height,
		DirectX::SimpleMath::Color color,
		DirectX::SimpleMath::Vector3 normal,
		bool xzflag = false);
	void CreateVertex();
	void CreateVertexXZ();
	void CreateIndex();
	// 幅
	float GetWidth();
	// 高さ
	float GetHeight();
	int GetDivX();
	int GetDivY();
	// 指定した3角形番号の三角形インデックスを取得
	FACE GetTriangle(int triangleno);

	// 何番目の四角形かを見つける
	int GetSquareNo(DirectX::SimpleMath::Vector3 pos);

private:
	unsigned int m_divX = 1;
	unsigned int m_divY = 1;
	float  m_height = 100.0f;
	float  m_width = 100.0f;
	DirectX::SimpleMath::Color m_color;
	DirectX::SimpleMath::Vector3 m_normal;
};



class CPlaneMeshWithoutCommon : public CMesh {
public:
	void Init(int divx, int divy,
		int width, int height,
		DirectX::SimpleMath::Color color,
		DirectX::SimpleMath::Vector3 normal);
	void CreateIndex();
	// 幅
	float GetWidth();
	// 高さ
	float GetHeight();
	int GetDivX();
	int GetDivY();

private:
	int m_divX = 1;
	int m_divY = 1;
	float  m_height = 100.0f;
	float  m_width = 100.0f;
	DirectX::SimpleMath::Color m_color;
	DirectX::SimpleMath::Vector3 m_normal;
};
