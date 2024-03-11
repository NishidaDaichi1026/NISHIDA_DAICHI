#include	<SimpleMath.h>
#include	"Collision.h"
#include	"CBoundingSphere.h"

using namespace DirectX::SimpleMath;

bool HitCheckSphere(const BoundingSphere& p1, const BoundingSphere& p2){

	double centerlength = (p1.Center - p2.Center).Length();
	double radiuslength = (p1.Radius + p2.Radius);

	if (centerlength <= radiuslength) {
		return true;
	}

	return false;
}

// 三角形の内部かどうかを判定する
bool CheckInTriangle(
	const Vector3& a,
	const Vector3& b,
	const Vector3& c,
	const Vector3& p) {

	Vector3	ab, bc, ca;			// ３辺のベクトル
	Vector3	ap, bp, cp;			// 内部の点とのベクトル
	Vector3	normal;				// ３角形の法線ベクトル
	Vector3	n1, n2, n3;			// ３辺と内部の点との法線ベクトル

	ab = b - a;

	bc = c - b;

	ca = a - c;

	ap = p - a;

	bp = p - b;

	cp = p - c;

	normal = ab.Cross(bc);

	n1 = ab.Cross(ap);

	n2 = bc.Cross(bp);

	n3 = ca.Cross(cp);

	float dot = n1.Dot(normal);
	if (dot < 0) return false;			// 為す角度が鈍角

	dot = n2.Dot(normal);
	if (dot < 0) return false;			// 為す角度が鈍角

	dot = n3.Dot(normal);
	if (dot < 0) return false;			// 為す角度が鈍角

	return true;
}

bool LinetoPlaneCross(
	const Plane& plane,		//平面の方程式
	const Vector3& p0,			//直線の起点
	const Vector3& wv,			//直線の方向ベクトル
	float& t,			// 交点位置情報	
	Vector3& ans)		//交点座標
{
	//	float t = 0;

	Vector3 normal;				// 平面の法線ベクトル（正規化済み）
	normal.x = plane.x;
	normal.y = plane.y;
	normal.z = plane.z;

	float dot;			// 分母
	
	dot = wv.Dot(normal);
	// 平行チェック(内積を計算する)
	if (fabsf(dot) < FLT_EPSILON) {
		//		MessageBox(nullptr, "平行です。", "平行です。", MB_OK);

		printf("平行です \n");
		return false;
	}

	// ここにｔを求める式を入れる
	float dot2;			// 分子
	dot2 = p0.Dot(normal);

	t = -(dot2 + plane.w) / dot;

	ans.x = p0.x + wv.x * t;
	ans.y = p0.y + wv.y * t;
	ans.z = p0.z + wv.z * t;
	return true;
}


