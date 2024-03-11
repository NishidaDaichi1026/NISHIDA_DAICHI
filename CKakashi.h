#pragma once
#include	<SimpleMath.h>
#include	"CShader.h"
#include	"CStaticMesh.h"
#include	"CStaticMeshRenderer.h"
#include	"State.h"
#include	"Field.h"
#include	"BattleScene.h"
#include	"XAudio2.h"

#include	"Player.h"
#include	"weapon.h"
#include	"FireBall.h"
#include	"NormalStar.h"

using namespace DirectX::SimpleMath;
//�X�e�[�^�X�̎擾
extern ST state[];
extern ATTRIBUTE attr[];

// ��
static CSphereMesh g_sphere;
// �}�e���A��
static CMaterial g_material;
// �`��ׂ̈ɕK�v�ȏ��
// �g�p����V�F�[�_�[
static CShader		g_shader;
// �X�^�e�B�b�N���b�V���i�W�I���g���f�[�^�j
static CStaticMesh g_staticmesh;
// ���b�V�������_���[
static CStaticMeshRenderer g_staticmeshrenderer;
// ���b�V�������_��
static CMeshRenderer g_meshrenderer;
// Bounding sphere
static CBoundingSphere g_bs;



class CKAKASHI {

	struct KakashiAbility {
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
	//--------------------------------
	KakashiAbility K_abi;
	//--------------------------------
	Field p_field;

	PlayerAbility* P_abi = GetPlayerAbi();
	Player* g_player = GetPlayer();
	CFireBall* fireball = GetFireBall();
	BattleScene* BScene = GetBattleScene();
	Weapon_ST* weapon_st = GetWeaponST();
	HitStop* h_stop = GetHitStop();
	//-------------------------------------------------------------------------------------------
	const	float RATE_ROTATE_ENEMY = 0.05f;			//��]�����W��
	const   float DISCOVERY_LENGTH = 300.0f;			//�����o������
	//-------------------------------------------------------------------------------------------
	float FromPlayerLength = 0.0f;		//���݂̃v���C���[�Ƃ̋���
	float S_Int_Leangth = 50.0f;		//�v���C���[�Əd�Ȃ�Ȃ��悤��(�����ȉ��܂ŋߊ��Ȃ�)
	int DAMAGE_CHACK = 0;				//0=�q�b�g���Ȃ��@1=�q�b�g����
	int ULT_HIT_INTERVAL = 0;			//�K�E�Z�̃q�b�g�Ԋu
	int d_HitInterval = 0;				//�ʏ�U���̃q�b�g�Ԋu�i�K�E�Z�Ɠ����ɍU���������̂Łj
	int NodamageTime = 1 * 60;			//�_���[�W�Ԋu�i�b�����t���[���j
	float KnockBackPower = 15.0f;		//�m�b�N�o�b�N�̋���(��)
	float KnockBackHeight = 30.0f;		//�m�b�N�o�b�N�̍���
	float KnockBackCount = 0.0f;		//�m�b�N�o�b�N�Ɏg�p

	// �v���C���̏��(S,R,T)------------------------------------------------------------------------
	DirectX::SimpleMath::Vector3	m_position = { 0.0f,0.0f,0.0f };		// �ʒu
	DirectX::SimpleMath::Vector3	m_rotation = { 0.0f,0.0f,0.0f };		// �p��
	DirectX::SimpleMath::Vector3	m_scale = { 1.5f,1.5f,1.5f };			// �g�k
	DirectX::SimpleMath::Vector3	m_move = { 0.0f,0.0f,0.0f };			// �ړ���
	DirectX::SimpleMath::Vector3	m_destrot = { 0.0f,0.0f,0.0f };			// �ڕW��]�p�x
	DirectX::SimpleMath::Matrix		m_mtx;
	//----------------------------------------------------------------------------------------------

	// �`��ׂ̈ɕK�v�ȏ��
	// �g�p����V�F�[�_�[
	CShader* m_shader = nullptr;
	// �X�^�e�B�b�N���b�V���i�W�I���g���f�[�^�j
	CStaticMesh* m_staticmesh = nullptr;
	// ���b�V�������_���[
	CStaticMeshRenderer* m_staticmeshrenderer = nullptr;


public:
	//--------------------------------------------------------------
	//���擾
	DirectX::SimpleMath::Vector3 GetPosition() {
		return m_position;
	}
	DirectX::SimpleMath::Vector3 GetScale() {
		return m_scale;
	}
	DirectX::SimpleMath::Vector3 GetMove() {
		return m_move;
	}
	DirectX::SimpleMath::Vector3 GetRotation() {
		return m_rotation;;
	}
	DirectX::SimpleMath::Matrix GetMatrix() {
		return m_mtx;
	}
	//--------------------------------------------------------------
	//�O������Z�b�g
	void SetShader(CShader* shader) {
		m_shader = shader;
	}
	void SetMesh(CStaticMesh* mesh) {
		m_staticmesh = mesh;
	}
	void SetMeshRenderer(CStaticMeshRenderer* meshrender) {
		m_staticmeshrenderer = meshrender;
	}
	void SetPosition(DirectX::SimpleMath::Vector3 pos) {
		m_position = pos;
	}

	void SetA_ID(int aid) {
		K_abi.A_ID = aid;
	}
	//--------------------------------------------------------------

	//�X�e�[�^�X������-----------------------------------------------------------------------
	void InitKakashi_ST() {

		//�e�l��State.cpp�Q��
		//��{�f�[�^
		K_abi.ID = state[4].ID;
		K_abi.name = state[K_abi.ID].name;
		//�X�e�[�^�X����
		K_abi.HP = state[K_abi.ID].HP;
		K_abi.ATTACK = state[K_abi.ID].ATTACK;
		K_abi.DEFENCE = state[K_abi.ID].DEFENCE;
		K_abi.SPEED = state[K_abi.ID].SPEED;

		//��������
		//K_abi.A_ID = attr[3].ID;
		K_abi.attrName = attr[K_abi.A_ID].attrName;
		K_abi.normal = attr[K_abi.A_ID].normal;
		K_abi.fire = attr[K_abi.A_ID].fire;
		K_abi.thunder = attr[K_abi.A_ID].thunder;
		K_abi.water = attr[K_abi.A_ID].water;
		K_abi.ice = attr[K_abi.A_ID].ice;

		m_rotation = { 0.2f,0.0f,0.0f };
		
	}

	//�X�V-----------------------------------------------------------------------------------
	void Update() {

		// �������擾
		float height;
		height = GetFieldHeight(m_position);

		// �v���C���̍��W���擾
		DirectX::SimpleMath::Vector3 playerpos = GetPositionPlayer();


		Battle();

		if (g_player->ULT_I == 1)return;		//�X�̃X�L���𔭓������瓮���Ȃ�����(����)
		// atan2���g�p���Ċp�x�����߂�
		m_destrot.y = atan2f(-(playerpos.x - m_position.x), -(playerpos.z - m_position.z));
		// ���݂̌����Ƃ̍������v�Z����
		float fDiffRotY = m_destrot.y - m_rotation.y;

		// �␳�i�|�P�W�O�`�P�W�O�͈̔́j
		if (fDiffRotY > DirectX::XM_PI) {
			fDiffRotY -= DirectX::XM_PI * 2.0f;
		}
		if (fDiffRotY < -DirectX::XM_PI) {
			fDiffRotY += DirectX::XM_PI * 2.0f;
		}

		// ��]�p�x�v�Z
		m_rotation.y += fDiffRotY * RATE_ROTATE_ENEMY;
		if (m_rotation.y > DirectX::XM_PI) {
			m_rotation.y -= DirectX::XM_PI * 2.0f;
		}
		if (m_rotation.y < -DirectX::XM_PI) {
			m_rotation.y += DirectX::XM_PI * 2.0f;
		}
	}

	//�o�g���̏���---------------------------------------------------------------------------
	void Battle() {
		// �v���C���̕���BS���擾
		BoundingSphere pbs = GetWeaponBS();			// ���S���W�̃��[���h�ϊ��s����擾
		BoundingSphere fbs = GetFireBallBS();
		BoundingSphere nsbs = GetNormalStarBS();

		Matrix smtx;
		smtx = GetMatrix();							// �G�̍s����擾
		BoundingSphere sbs;

		sbs = g_bs.MakeBS(smtx, GetScale());

		//�v���C���[�̍U���q�b�g����
		bool sts = HitCheckSphere(pbs, sbs);
		//�����Ă���&&�_���[�W�Ԋu���Ƃ�Ă���&&����̓����蔻�肪�����Ă���
		if (sts && DAMAGE_CHACK == 0 && weapon_st->WeaponHitCheck == 1) {
			DamageCalc();
			DAMAGE_CHACK = 1;
			h_stop->hitchack = true;
			PlaySound(SE7);
		}
		if (DAMAGE_CHACK == 1) {
			d_HitInterval += 1;
			m_rotation.y += 0.5f;
			if (d_HitInterval >= NodamageTime) {
				DAMAGE_CHACK = 0;
				d_HitInterval = 0;
			}
		}

		//�΂̕K�E�Z�ɑ΂��鏈��
		bool usts = HitCheckSphere(fbs, sbs);
		if (g_player->ULT_F == 1) {
			ULT_HIT_INTERVAL++;
			if (usts && ULT_HIT_INTERVAL >= NodamageTime) {
				ULTDamageCalc();
				h_stop->hitchack = true;
				ULT_HIT_INTERVAL = 0;
			}
		}

		//�������̕K�E�Z�ɑ΂��鏈��
		bool nssts = HitCheckSphere(nsbs, sbs);
		if (g_player->ULT_N == 1) {
			K_abi.DEFENCE /= 10;						//�������Ă���Ɩh��͌�
		}
		else {
			K_abi.DEFENCE = state[K_abi.ID].DEFENCE;	//�����Ă��Ȃ��Ə����l������
		}
		//�������̕K�E�Z�ɑ΂��鏈���i���j
		if (g_player->ULT_T == 1 && DAMAGE_CHACK == 0
			&& FromPlayerLength <= DISCOVERY_LENGTH) {
			DAMAGE_CHACK = 1;
			ULTDamageCalc();
			h_stop->hitchack = true;
		}
	}

	//�v���C���[�̒ʏ�U���̌v�Z-------------------------------------------------------------
	void DamageCalc() {
		P_abi->DAMAGE = P_abi->ATTACK - K_abi.DEFENCE;

		//�������ɔ{���ύX
		switch (P_abi->A_ID) {
		case 0:
			P_abi->DAMAGE = P_abi->DAMAGE * K_abi.normal;
			break;
		case 1:
			P_abi->DAMAGE = P_abi->DAMAGE * K_abi.fire;
			break;
		case 2:
			P_abi->DAMAGE = P_abi->DAMAGE * K_abi.water;
			break;
		case 3:
			P_abi->DAMAGE = P_abi->DAMAGE * K_abi.ice;
			break;
		case 4:
			P_abi->DAMAGE = P_abi->DAMAGE * K_abi.thunder;
			break;
		default:
			break;
		}
		//�`���[�W�o���Ă�����_���[�W�ɔ{��
		if (weapon_st->Max_charge == true) {
			P_abi->DAMAGE *= weapon_st->chargeMag;
		}
	}

	//�v���C���[�̕K�E�Z�̌v�Z---------------------------------------------------------------
	void ULTDamageCalc() {
		P_abi->DAMAGE = P_abi->ATTACK - K_abi.DEFENCE;

		//�������ɔ{���ύX
		switch (P_abi->A_ID) {
		case 0:
			P_abi->DAMAGE = P_abi->DAMAGE * K_abi.normal;
			break;
		case 1:
			P_abi->DAMAGE = P_abi->DAMAGE * K_abi.fire;
			break;
		case 2:
			P_abi->DAMAGE = P_abi->DAMAGE * K_abi.water;
			break;
		case 3:
			P_abi->DAMAGE = P_abi->DAMAGE * K_abi.ice;
			break;
		case 4:
			P_abi->DAMAGE = P_abi->DAMAGE * K_abi.thunder;
			break;
		default:
			break;
		}
		P_abi->DAMAGE *= 2;
	}

	// �`��----------------------------------------------------------------------------------
	void Draw() {

		//��苗�����̂ݕ\��
		if (FromPlayerLength >= 700)return;
		// �f�o�C�X�R���e�L�X�g�擾
		ID3D11DeviceContext* devicecontext;
		devicecontext = Renderer::GetDeviceContext();

		// ���[���h�ϊ��s�񐶐�
		DirectX::SimpleMath::Matrix mtx;
		DX11MakeWorldMatrixRadian(
			mtx,
			m_scale,							// �X�P�[��
			m_rotation,							// �p��
			m_position);						// �ʒu

		m_mtx = mtx;

		// GPU�ɍs����Z�b�g����
		Renderer::SetWorldMatrix(&mtx);

		// �V�F�[�_�[��GPU�ɃZ�b�g
		m_shader->SetGPU();

		// ���f���`��
		m_staticmeshrenderer->Draw();

	}
};