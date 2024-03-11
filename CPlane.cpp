#include	<SimpleMath.h>
#include	"CPlane.h"

// 平面の方程式を作成する
void CPlane::MakeEquatation(
	DirectX::SimpleMath::Vector3 p0,
	DirectX::SimpleMath::Vector3 p1,
	DirectX::SimpleMath::Vector3 p2)
{
	DirectX::SimpleMath::Plane p;
	DirectX::SimpleMath::Vector3		p0p1;
	DirectX::SimpleMath::Vector3		p1p2;
	DirectX::SimpleMath::Vector3		normal;

	p0p1 = p1 - p0;

	p1p2 = p2 - p1;

	normal = p0p1.Cross(p1p2);

	normal.Normalize();

//	DX11Vec3Cross(normal, p0p1, p1p2);

//	DX11Vec3Normalize(normal, normal);

	// a,b,c,dを求める
	p.x = normal.x;
	p.y = normal.y;
	p.z = normal.z;
	p.w = -(p.x * p0.x + p.y * p0.y + p.z * p0.z);

	// メンバ変数へセット
	m_planeinfo.plane = p;

	m_planeinfo.p0 = p0;
	m_planeinfo.p1 = p1;
	m_planeinfo.p2 = p2;
}