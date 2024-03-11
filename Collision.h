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
	const DirectX::SimpleMath::Plane& plane,		//���ʂ̕�����
	const DirectX::SimpleMath::Vector3& p0,			//�����̋N�_
	const DirectX::SimpleMath::Vector3& wv,			//�����̕����x�N�g��
	float& t,			// ��_�ʒu���	
	DirectX::SimpleMath::Vector3& ans);		//��_���W
