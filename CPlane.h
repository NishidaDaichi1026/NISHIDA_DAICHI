#pragma once
#include	<SimpleMath.h>

// 地形データの面の方程式情報
struct PLANEINFO {
	DirectX::SimpleMath::Plane		plane;			//平面の方程式
	DirectX::SimpleMath::Vector3	p0, p1, p2;		//頂点座標
};

// 平面クラス
class CPlane {
	PLANEINFO	m_planeinfo;
public:
	void MakeEquatation(
			DirectX::SimpleMath::Vector3 p0,
			DirectX::SimpleMath::Vector3 p1,
			DirectX::SimpleMath::Vector3 p2
	);
	PLANEINFO GetPlaneInfo() {
		return m_planeinfo;
	}
};

