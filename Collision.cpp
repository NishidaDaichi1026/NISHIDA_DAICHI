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

// �O�p�`�̓������ǂ����𔻒肷��
bool CheckInTriangle(
	const Vector3& a,
	const Vector3& b,
	const Vector3& c,
	const Vector3& p) {

	Vector3	ab, bc, ca;			// �R�ӂ̃x�N�g��
	Vector3	ap, bp, cp;			// �����̓_�Ƃ̃x�N�g��
	Vector3	normal;				// �R�p�`�̖@���x�N�g��
	Vector3	n1, n2, n3;			// �R�ӂƓ����̓_�Ƃ̖@���x�N�g��

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
	if (dot < 0) return false;			// �ׂ��p�x���݊p

	dot = n2.Dot(normal);
	if (dot < 0) return false;			// �ׂ��p�x���݊p

	dot = n3.Dot(normal);
	if (dot < 0) return false;			// �ׂ��p�x���݊p

	return true;
}

bool LinetoPlaneCross(
	const Plane& plane,		//���ʂ̕�����
	const Vector3& p0,			//�����̋N�_
	const Vector3& wv,			//�����̕����x�N�g��
	float& t,			// ��_�ʒu���	
	Vector3& ans)		//��_���W
{
	//	float t = 0;

	Vector3 normal;				// ���ʂ̖@���x�N�g���i���K���ς݁j
	normal.x = plane.x;
	normal.y = plane.y;
	normal.z = plane.z;

	float dot;			// ����
	
	dot = wv.Dot(normal);
	// ���s�`�F�b�N(���ς��v�Z����)
	if (fabsf(dot) < FLT_EPSILON) {
		//		MessageBox(nullptr, "���s�ł��B", "���s�ł��B", MB_OK);

		printf("���s�ł� \n");
		return false;
	}

	// �����ɂ������߂鎮������
	float dot2;			// ���q
	dot2 = p0.Dot(normal);

	t = -(dot2 + plane.w) / dot;

	ans.x = p0.x + wv.x * t;
	ans.y = p0.y + wv.y * t;
	ans.z = p0.z + wv.z * t;
	return true;
}

