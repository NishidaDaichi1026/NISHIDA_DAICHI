#pragma once
#include	<directxmath.h>
#include	"3DModel.h"
#include	"CPlaneMesh.h"
#include	"CMeshRenderer.h"
#include	"CBoundingSphere.h"
#include	"CSphereMesh.h"
#include	"Field.h"
#include	"TrainingRoomFloor.h"

constexpr	float VALUE_MOVE_MODEL = 0.5f;						// �ړ��X�s�[�h
constexpr	float VALUE_ROTATE_MODEL = DirectX::XM_PI * 0.02f;	// ��]�X�s�[�h
constexpr	float RATE_ROTATE_MODEL = 0.40f;					// ��]�����W��
constexpr	float RATE_MOVE_MODEL = 0.20f;						// �ړ������W��

enum class PlayerState {
	LIVE,		//�����Ă���
	DEAD		//����ł���
};

struct Player{
	PlayerState state = PlayerState::LIVE;
	float PossibleTime = 5.0;			//�X�L���N�[���^�C��
	float P_sec = 0.0f;					//�X�L���̃N�[���^�C���Ɏg�p
	clock_t start = timeGetTime();		//�X�L���̃N�[���^�C���Ɏg�p
	float playerlimit = 20.0f;			//�͈͎擾
	bool ThunderSkillChack = false;		//���̃X�L���ŉ�ʂ�h�炷�̂Ŏg�������̃`�F�b�N
	//����ID-----------------------
	const int NORMAL_ID = 0;
	const int FIRE_ID = 1;
	const int WATER_ID = 2;
	const int ICE_ID = 3;
	const int THUNDER_ID = 4;
	//�`���[�W�ϐ�-----------------
	int charge_F = 0;
	int charge_N = 0;
	int charge_W = 0;
	int charge_I = 0;
	int charge_T = 0;
	//�K�v�`���[�W��---------------
	int NeedCharge = 3;
	//�X�L���t���O-----------------
	int ULT_F = 0;
	int ULT_N = 0;
	int ULT_W = 0;
	int ULT_I = 0;
	int ULT_T = 0;
	//�X�L������-------------------
	int F_Time = 0;
	int N_Time = 0;
	int W_Time = 0;
	int I_Time = 0;
	int T_Time = 0;
	//�X�L���p������---------------
	int SkillDuration = 10 * 60;	//���ԁ��t���[��
};

struct PlayerAbility
{
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

// �v���g�^�C�v�錾----------------------------------------

void InitPlayer();				//������
void PlayerULT();				//�K�E�Z�̃��f���̏�����
void InitPlayerState();			//�X�e�[�^�X������
void UpdatePlayer();			//�X�V
void MovePlayer();				//�ړ�
void UpdatePlayerBattle();		//�o�g���̏����i�K�E�Z�j
void PlayerAbiChange();			//�����̕ύX����
void DrawPlayer();				//�`��
void ExitPlayer();				//���������
//���擾-------------------------------------------------
// �ʒu�擾
DirectX::XMFLOAT3 GetPositionPlayer();
// �p���擾
DirectX::XMFLOAT3 GetRotationPlayer();
// �ړ��ʎ擾
DirectX::XMFLOAT3 GetMovePlayer();
// �v���C��BS�擾
BoundingSphere GetPlayerBS();

PlayerAbility* GetPlayerAbi();
Player* GetPlayer();
//�O������Z�b�g-------------------------------------------
void SetPlayerPosition(DirectX::SimpleMath::Vector3 pos);
void SetPlayerRotation(DirectX::SimpleMath::Vector3 rot);