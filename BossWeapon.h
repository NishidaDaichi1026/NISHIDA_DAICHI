#pragma once
#include	<DirectXMath.h>
#include	"3DModel.h"
#include	"CMeshRenderer.h"
#include	"CBoundingSphere.h"
#include	"CSphereMesh.h"

struct BossWeapon_ST {
	int WeaponHitCheck = 0;	//�����蔻��̗L���A���������߂�@0=�����@�@1=�L��	
	int attcheck = 0;		//�U���̃A�j���[�V���������邩	  0=���Ȃ��@1=����
	float AttAnimeTime = 0.7f * 60;	//�U���̃A�j���[�V��������(�b���t���[��)
	float attTime = 0;		//�U���̃A�j���[�V�����Ɏg�p����̂�float

	int BossAIChack = 0;	//�{�X�̍U���Ԋu�̃J�E���g�����
	const int BossAttInterval = 3 * 60;		//�{�X�̍U���Ԋu�����߂�(�b���t���[��)
};

//�{�X�̕���̏��擾-------------------------------
BoundingSphere GetBossWeaponBS();
BossWeapon_ST* GetBossWeaponST();
DirectX::SimpleMath::Vector3 GetBWPosition();
DirectX::SimpleMath::Vector3 GetBWScale();
DirectX::SimpleMath::Vector3 GetBWMove();
DirectX::SimpleMath::Vector3 GetBWRotation();
DirectX::SimpleMath::Matrix GetBWMatrix();
//---------------------------------------------------
//������
void InitBossWeapon();
//�X�V
void UpdateBossWeapon();
//�_���[�W�v�Z
void BossDamageCalc();
//�`��
void DrawBossWeapon();
//���������
void ExitBossWeapon();
