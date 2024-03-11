#include	"BossEnemy.h"
#include	"Field.h"
#include	"BossWeapon.h"
#include	"State.h"
#include	"XAudio2.h"

//�������蔻��Ɏg�p
#include	"Player.h"
#include	"weapon.h"
#include	"FireBall.h"
#include	"NormalStar.h"

using namespace DirectX::SimpleMath;

// �v���C���̏��(S,R,T)
static Vector3	g_position = { 0.0f,0.0f,200.0f };		// �ʒu
static Vector3	g_rotation = { 0.0f,0.0f,0.0f };		// �p��
static Vector3	g_scale = { 0.2f,0.2f,0.2f };			// �g�k

// Bounding sphere
static CBoundingSphere g_bs;
// ��
static CSphereMesh g_sphere;
// ���b�V�������_��
static CMeshRenderer g_meshrenderer;
// �}�e���A��
static CMaterial g_material;

// �`��ׂ̈ɕK�v�ȏ��
// �g�p����V�F�[�_�[
static CShader		g_shader;

// �X�^�e�B�b�N���b�V���i�W�I���g���f�[�^�j
static CStaticMesh g_staticmesh;

// ���b�V�������_���[
static CStaticMeshRenderer g_staticmeshrenderer;

static Vector3	g_move = { 0.0f,0.0f,0.0f };			// �ړ���
static Vector3	g_destrot = { 0.0f,0.0f,0.0f };			// �ڕW��]�p�x
DirectX::XMFLOAT3 bossTrans(0, 0, 0);

DirectX::SimpleMath::Matrix		b_mtx;

const	float RATE_ROTATE_BOSS = 0.05f;

Boss_ST boss_st;
BossAbility B_abi;

Field bs_field;

//�X�e�[�^�X�̎擾
extern ST state[];
extern ATTRIBUTE attr[];

//-----------------------------------------------
//�{�X�̏����擾
Boss_ST* GetBossST() {
	return &boss_st;
}
BossAbility* GetBossABI() {
	return &B_abi;
}
DirectX::XMFLOAT3 GetBossPosition() {
	return g_position;
}
DirectX::XMFLOAT3 GetBossRotation() {
	return g_rotation;
}
DirectX::XMFLOAT3 GetBossScale() {
	return g_scale;
}
DirectX::XMFLOAT3 GetBossMove() {
	return g_move;
}
DirectX::SimpleMath::Matrix GetBossMatrix() {
	return b_mtx;
}
//-----------------------------------------------

BoundingSphere GetBossBS() {

	// ���[���h�ϊ��s�񐶐�
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_scale,							// �X�P�[��
		g_rotation,							// �p��
		g_position);						// �ʒu

	// BS����
	BoundingSphere bs;
	bs = g_bs.MakeBS(mtx, g_scale);

	return bs;
}

void InitBoss()
{
	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// ���_�V�F�[�_
		"shader/vertexLightingPS.hlsl");				// �s�N�Z���V�F�[�_

	// ���f���t�@�C����
	std::string filename[] = {
		u8"assets\\model\\mino.fbx",
	};

	// ���b�V�������i�W�I���g���f�[�^)
	g_staticmesh.Init(filename[0]);

	// �`��ׂ̈̃f�[�^����
	g_staticmeshrenderer.Init(g_staticmesh);

	// ���E�������߂�
	g_bs.Caliculate(g_staticmesh.GetVertices());

	g_sphere.Init(g_bs.GetRadius(), Color(1, 1, 1, 1), 100, 100);
	g_meshrenderer.Init(g_sphere);

	MATERIAL mtrl;
	mtrl.Ambient = Color(0, 0, 0, 0);
	mtrl.Diffuse = Color(1, 1, 1, 0.3f);
	mtrl.Specular = Color(0, 0, 0, 0);
	mtrl.Shininess = 0;
	mtrl.Emission = Color(0, 0, 0, 0);
	mtrl.TextureEnable = FALSE;

	g_material.Init(mtrl);

	InitBossWeapon();
	InitBoss_ST();
}

void InitBoss_ST() {

	boss_st.B_sts = boss_st.Live;
	//�e�l��State.cpp�Q��
	//��{�f�[�^
	B_abi.ID = state[3].ID;
	B_abi.name = state[B_abi.ID].name;
	//�X�e�[�^�X����
	B_abi.HP = state[B_abi.ID].HP;
	B_abi.ATTACK = state[B_abi.ID].ATTACK;
	B_abi.DEFENCE = state[B_abi.ID].DEFENCE;
	B_abi.SPEED = state[B_abi.ID].SPEED;

	//��������
	B_abi.A_ID = attr[0].ID;
	B_abi.attrName = attr[B_abi.A_ID].attrName;
	B_abi.normal = attr[B_abi.A_ID].normal;
	B_abi.fire = attr[B_abi.A_ID].fire;
	B_abi.thunder = attr[B_abi.A_ID].thunder;
	B_abi.water = attr[B_abi.A_ID].water;
	B_abi.ice = attr[B_abi.A_ID].ice;

	//�����ʒu
	g_position = { 0.0f,0.0f,200.0f };
	g_rotation = { 0.0f,0.0f,0.0f };
}

void ExitBoss() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
	ExitBossWeapon();
}

void UpdateBoss() {

	BossBattle();

	Player* g_player = GetPlayer();
	if (g_player->ULT_I == 1)return;		//�X�̃X�L���𔭓������瓮���Ȃ�����(����)
	
	//�{�X�̕���X�V
	UpdateBossWeapon();

	// �v���C���̍��W���擾
	DirectX::SimpleMath::Vector3 playerpos = GetPositionPlayer();

	// atan2���g�p���Ċp�x�����߂�
	g_destrot.y = atan2f(-(playerpos.x - g_position.x), -(playerpos.z - g_position.z));

	// ���݂̌����Ƃ̍������v�Z����
	float fDiffRotY = g_destrot.y - g_rotation.y;

	//�v���C���[�Ƃ̋������擾
	boss_st.FromPlayerLength = (playerpos - g_position).Length();

	//�_���[�W���m�b�N�o�b�N
		//�m�b�N�o�b�N�������Œ肷�邽�߂ɁA�_���[�W����g_move���X�V���Ȃ�
	if (boss_st.DAMAGE_CHACK == 1) {
		g_position -= (g_move / boss_st.FromPlayerLength) * boss_st.KnockBackPower;
	}
	else {
		//�ړ��ʎ擾
		g_move = playerpos - g_position;
	}

	if (boss_st.FromPlayerLength <= boss_st.S_Int_Leangth)return;
	g_position += (g_move / boss_st.FromPlayerLength) * B_abi.SPEED;

	// atan2���g�p���Ċp�x�����߂�
	g_destrot.y = atan2f(-(playerpos.x - g_position.x), -(playerpos.z - g_position.z));

	// �␳�i�|�P�W�O�`�P�W�O�͈̔́j
	if (fDiffRotY > DirectX::XM_PI)
	{
		fDiffRotY -= DirectX::XM_PI * 2.0f;
	}
	if (fDiffRotY < -DirectX::XM_PI)
	{
		fDiffRotY += DirectX::XM_PI * 2.0f;
	}

	// ��]�p�x�v�Z
	g_rotation.y += fDiffRotY * RATE_ROTATE_BOSS;
	if (g_rotation.y > DirectX::XM_PI)
	{
		g_rotation.y -= DirectX::XM_PI * 2.0f;
	}
	if (g_rotation.y < -DirectX::XM_PI)
	{
		g_rotation.y += DirectX::XM_PI * 2.0f;
	}


	// �͈̓`�F�b�N
	if (g_position.x <= -bs_field.FieldX_MAX / 2.5f) {
		g_position.x = -bs_field.FieldX_MAX / 2.5f;
	}
	if (g_position.x >= bs_field.FieldX_MAX / 2.5f) {
		g_position.x = bs_field.FieldX_MAX / 2.5f;
	}
	if (g_position.z >= bs_field.FieldZ_MAX / 2.5f) {
		g_position.z = bs_field.FieldZ_MAX / 2.5f;
	}
	if (g_position.z <= -bs_field.FieldZ_MAX / 2.5f) {
		g_position.z = -bs_field.FieldZ_MAX / 2.5f;
	}
	
}

void BossBattle() {
	float height;
	// �������擾
	height = GetFieldHeight(g_position);
	g_position.y = height + g_scale.y;

	Player* g_player = GetPlayer();
	Weapon_ST* weapon_st = GetWeaponST();
	HitStop* h_stop = GetHitStop();

	// �v���C���̍U���Ɏg�p����BS���擾
	BoundingSphere pbs = GetWeaponBS();
	BoundingSphere fbs = GetFireBallBS();
	BoundingSphere sbs = GetNormalStarBS();

	//�v���C���[�̍U���q�b�g����
	bool sts = HitCheckSphere(pbs, GetBossBS());
	if (sts && boss_st.DAMAGE_CHACK == 0 && weapon_st->WeaponHitCheck == 1) {
		BossDamageHitCalc();
		boss_st.DAMAGE_CHACK = 1;
		h_stop->hitchack = true;
		PlaySound(SE7);
	}
	//��莞�ԃ_���[�W���󂯂Ȃ�
	if (boss_st.DAMAGE_CHACK == 1) {
		boss_st.d_HitInterval++;
		if (boss_st.d_HitInterval >= boss_st.NodamageTime) {
			boss_st.DAMAGE_CHACK = 0;
			boss_st.d_HitInterval = 0;
		}
	}

	//�΂̕K�E�Z�ɑ΂��鏈��
	bool usts = HitCheckSphere(fbs, GetBossBS());
	if (g_player->ULT_F == 1) {
		boss_st.ULT_HIT_INTERVAL++;
		if (usts && boss_st.ULT_HIT_INTERVAL >= boss_st.NodamageTime) {
			BossULTDamageHitCalc();
			h_stop->hitchack = true;
			boss_st.ULT_HIT_INTERVAL = 0;
		}
	}
	//�������̕K�E�Z�ɑ΂��鏈��
	bool nssts = HitCheckSphere(sbs, GetBossBS());
	if (g_player->ULT_N == 1) {
		B_abi.DEFENCE /= 10;			//�������Ă���Ɩh��͌�
	}
	else {
		B_abi.DEFENCE = state[B_abi.ID].DEFENCE;	//�����Ă��Ȃ��Ə����l������
	}

	//�������̕K�E�Z�ɑ΂��鏈���i���j
	if (g_player->ULT_T == 1 && boss_st.DAMAGE_CHACK == 0) {
		boss_st.DAMAGE_CHACK = 1;
		BossULTDamageHitCalc();
		h_stop->hitchack = true;
	}
	//�G��|������
	if (B_abi.HP <= 0) {
		boss_st.B_sts = boss_st.Dead;
	}
}

//�v���C���[�̒ʏ�U���̌v�Z
void BossDamageHitCalc() {
	PlayerAbility* P_abi = GetPlayerAbi();
	Weapon_ST* weapon_st = GetWeaponST();
	//�v���C���[�̍U���͂ƃG�l�~�[�̖h��͂���_���[�W���v�Z
	P_abi->DAMAGE = P_abi->ATTACK - B_abi.DEFENCE;
	//�v���C���[�̑����Ɖ����������ϐ��̔{�����_���[�W�Ɋ|����
	switch (P_abi->A_ID) {
	case 0:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.normal;
		break;
	case 1:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.fire;
		break;
	case 2:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.water;
		break;
	case 3:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.ice;
		break;
	case 4:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.thunder;
		break;
	default:
		break;
	}

	//�`���[�W�o���Ă�����_���[�W�ɔ{��
	if (weapon_st->Max_charge == true) {
		P_abi->DAMAGE *= weapon_st->chargeMag;
	}

	//�_���[�W��^����
	B_abi.HP = B_abi.HP - P_abi->DAMAGE;
}

//�v���C���[�̕K�E�Z�̌v�Z
void BossULTDamageHitCalc() {
	PlayerAbility* P_abi = GetPlayerAbi();
	//�v���C���[�̍U���͂ƃG�l�~�[�̖h��͂���_���[�W���v�Z
	P_abi->DAMAGE = P_abi->ATTACK - B_abi.DEFENCE;
	//�v���C���[�̑����Ɖ����������ϐ��̔{�����_���[�W�Ɋ|����
	switch (P_abi->A_ID) {
	case 0:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.normal;
		break;
	case 1:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.fire;
		break;
	case 2:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.water;
		break;
	case 3:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.ice;
		break;
	case 4:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.thunder;
		break;
	default:
		break;
	}
	P_abi->DAMAGE *= 2;
	//�_���[�W��^����
	B_abi.HP = B_abi.HP - P_abi->DAMAGE;
}

void DrawBoss()
{
	//����ł�����`�悵�Ȃ�
	if (boss_st.B_sts == boss_st.Dead)return;
	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// ���[���h�ϊ��s�񐶐�
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_scale,							// �X�P�[��
		g_rotation,							// �p��
		g_position);						// �ʒu

	// GPU�ɍs����Z�b�g����
	Renderer::SetWorldMatrix(&mtx);

	// �V�F�[�_�[��GPU�ɃZ�b�g
	g_shader.SetGPU();

	// ���f���`��
	g_staticmeshrenderer.Draw();

	//�{�X�̕���\��
	DrawBossWeapon();
}