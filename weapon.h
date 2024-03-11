#pragma once

#include	<DirectXMath.h>
#include	"3DModel.h"
#include	"CMeshRenderer.h"
#include	"CBoundingSphere.h"
#include	"CSphereMesh.h"

struct Weapon_ST{
	int WeaponHitCheck = 0;				//�����蔻��L�聁�P�������O
	float attTime = 0.0f;				//�U���̃A�j���[�V�����Ɏg�p
	float attackCoolTime = 0.5f * 60;	//�U���̃N�[���^�C���@�b�����t���[��
	//------------------------------------------------------------------------------
	float chargeTime = 1.0f * 60;		//�`���[�W�U���\����
	float charge = 0.0f;				//�`���[�W����
	//�O���Ɏ����Ă���
	float chargeMag = 1.8f;				//�`���[�W�U���{��
	float chargeAtMove = 2.0f;			//�`���[�W�U���̈ړ��{��
	bool Max_charge = false;			//���܂��Ă��邩
	//------------------------------------------------------------------------------
};

//�q�b�g�X�g�b�v
struct HitStop {
	bool hitchack = false;			//�G�ɓ������true�ŕԂ�
	float hitstopTime = 0.1f * 60;	//�~�߂����b�����t���[��
	float hitstopCnt = 0.0f;		//�b���J�E���g����ϐ�
};

//���擾-----------------------------------
BoundingSphere GetWeaponBS();
Weapon_ST* GetWeaponST();
HitStop* GetHitStop();
DirectX::XMFLOAT3 GetPositionWeapon();
//�v���g�^�C�v�錾---------------------------
void InitWeapon();			//������
void ExitWeapon();			//���������
void UpdateWeapon();		//�X�V
void DrawWeapon();			//�`��
//-------------------------------------------