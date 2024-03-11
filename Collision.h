#pragma once
#include	<SimpleMath.h>

struct BoundingSphere {
	DirectX::SimpleMath::Vector3 Center;
	float Radius;
};

bool HitCheckSphere(const BoundingSphere& p1, const BoundingSphere& p2);

bool CheckInTriangle(
	const DirectX::SimpleMath::Vector3& a,
	const DirectX::SimpleMath::Vector3& b,
	const DirectX::SimpleMath::Vector3& c,
	const DirectX::SimpleMath::Vector3& p);

bool LinetoPlaneCross(
	const DirectX::SimpleMath::Plane& plane,		//平面の方程式
	const DirectX::SimpleMath::Vector3& p0,			//直線の起点
	const DirectX::SimpleMath::Vector3& wv,			//直線の方向ベクトル
	float& t,			// 交点位置情報	
	DirectX::SimpleMath::Vector3& ans);		//交点座標
