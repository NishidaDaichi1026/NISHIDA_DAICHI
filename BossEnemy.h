#pragma once

#include	<DirectXMath.h>
#include	"3DModel.h"
#include	"CMeshRenderer.h"
#include	"CBoundingSphere.h"
#include	"CSphereMesh.h"

struct Boss_ST {
	//�{�X�̃X�e�[�^�X----------
	//�����X�e�[�^�X
	int B_sts = 1;
	//--------------------------
	int Live = 1;	//�����Ă���
	int Dead = 2;	//����ł���
	//--------------------------
	
	float FromPlayerLength = 0.0f;		//�v���C���[�Ƃ̋���
	int S_Int_Leangth = 40;			//�v���C���[�Əd�Ȃ�Ȃ��悤��(�����ȉ��܂ŋߊ��Ȃ�)
	int DAMAGE_CHACK = 0;			//�_���[�W���󂯂����̃`�F�b�N
	int ULT_HIT_INTERVAL = 0;		//�K�E�Z�̃q�b�g�Ԋu
	float KnockBackPower = 5.0f;	//�m�b�N�o�b�N�̋���(��)
	
	//�_���[�W�̊Ԋu--------------------
	float d_HitInterval = 0.0f;
	//�b�����t���[��
	float NodamageTime = 0.5f * 60;
	//----------------------------------
};

//�{�X�̔\�̓f�[�^
struct BossAbility {
	//��{�f�[�^
	int ID;
	const char* name;
	//�X�e�[�^�X
	int HP;
	int ATTACK;
	int DEFENCE;
	float SPEED;
	int DAMAGE;
	//����
	int A_ID;
	const char* attrName;
	//��������
	float normal;
	float fire;
	float water;
	float ice;
	float thunder;
};

//�{�X�̏��擾--------------------------------
BoundingSphere GetBossBS();
Boss_ST* GetBossST();
BossAbility* GetBossABI();
DirectX::XMFLOAT3 GetBossPosition();
DirectX::XMFLOAT3 GetBossRotation();
DirectX::XMFLOAT3 GetBossMove();
DirectX::XMFLOAT3 GetBossScale();
DirectX::SimpleMath::Matrix GetBossMatrix();
//----------------------------------------------

//������
void InitBoss();
//�X�e�[�^�X������
void InitBoss_ST();
//�X�V
void UpdateBoss();
//�_���[�W�v�Z�E�_���[�W�Ԋu�Ȃ�----------------
void BossBattle();
void BossDamageHitCalc();
void BossULTDamageHitCalc();
//----------------------------------------------
//�`��
void DrawBoss();
//���������
void ExitBoss();