#include	"Player.h"
#include	"CBoundingSphere.h"
#include	"State.h"
#include	"FireBall.h"
#include	"NormalStar.h"
#include	"weapon.h"
#include	"sceneChange.h"
#include	"SnowParticle.h"
#include	"XAudio2.h"

using namespace DirectX::SimpleMath;

// �v���C���̏��(S,R,T)
static Vector3	g_position = { 0.0f,0.0f,0.0f };		// �ʒu
static Vector3	g_rotation = { 0.0f,0.0f,0.0f };		// �p��
static Vector3	g_scale = { 15.0f,15.0f,15.0f };		// �g�k

// �`��ׂ̈ɕK�v�ȏ��
// �g�p����V�F�[�_�[
static CShader		g_shader;

// �X�^�e�B�b�N���b�V���i�W�I���g���f�[�^�j
static CStaticMesh g_staticmesh;

// Bounding sphere
static CBoundingSphere g_bs;
// ��
static CSphereMesh g_sphere;
// ���b�V�������_��
static CMeshRenderer g_meshrenderer;
// �}�e���A��
static CMaterial g_material;

// ���b�V�������_���[
static CStaticMeshRenderer g_staticmeshrenderer;

static Vector3  g_attackmove = { 0.0f,0.0f,0.0f };		//�U�����̈ړ�
static Vector3	g_move = { 0.0f,0.0f,0.0f };			//�ړ���
static Vector3  g_movechack = { 1.5f,1.5f,1.5f };		//�ړ����Ă��邩�̃`�F�b�N
static Vector3	g_destrot = { 0.0f,0.0f,0.0f };			// �ڕW��]�p�x

DirectX::XMFLOAT3 Playertrans(0, 0, 0);

//-----------------------------------------------------------
Player g_player;
PlayerAbility P_abi;
CFireBall fireball;
CNormalStar NStar;
Field p_field;
TRoom p_troom;
//-----------------------------------------------------------

//�X�e�[�^�X�̎擾
extern ST state[];
extern ATTRIBUTE attr[];

//-----------------------------------------------------------
// �ʒu�擾
DirectX::XMFLOAT3 GetPositionPlayer() {
	return g_position;
}
// �p���擾
DirectX::XMFLOAT3 GetRotationPlayer() {
	return g_rotation;
}
// �ړ��ʎ擾
DirectX::XMFLOAT3 GetMovePlayer() {
	return g_move;
}
//�@���擾
PlayerAbility* GetPlayerAbi() {
	return &P_abi;
}
Player* GetPlayer() {
	return &g_player;
}

//-----------------------------------------------------------
//�Z�b�g
void SetPlayerPosition(DirectX::SimpleMath::Vector3 pos) {
	g_position = pos;
}
void SetPlayerRotation(DirectX::SimpleMath::Vector3 rot) {
	g_rotation = rot;
}
//-----------------------------------------------------------
BoundingSphere GetPlayerBS() {

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

// ������
void InitPlayer() {

	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// ���_�V�F�[�_
		"shader/vertexLightingPS.hlsl");				// �s�N�Z���V�F�[�_

	// ���f���t�@�C����
	std::string filename[] = {
		u8"assets\\model\\Player\\Player.fbx",
	};

	// ���b�V�������i�W�I���g���f�[�^�j
	g_staticmesh.Init(filename[0]);

	// �`��ׂ̈̃f�[�^����
	g_staticmeshrenderer.Init(g_staticmesh);

	InitPlayerState();

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

	PlayerULT();
	// �p�[�e�B�N��������
	InitSnowParticle();
}
//�K�E�Z������
void PlayerULT() {
	fireball.Init();
	NStar.Init();
}
//�X�e�[�^�X������
void InitPlayerState() {
	//�e�l��State.cpp�Q��
	//��{�f�[�^
	P_abi.ID = state[0].ID;
	P_abi.name = state[P_abi.ID].name;
	//�X�e�[�^�X����
	P_abi.HP = state[P_abi.ID].HP;
	P_abi.ATTACK = state[P_abi.ID].ATTACK;
	P_abi.DEFENCE = state[P_abi.ID].DEFENCE;
	P_abi.SPEED = state[P_abi.ID].SPEED;
	//��������
	P_abi.A_ID = attr[0].ID;
	P_abi.attrName = attr[P_abi.A_ID].attrName;
	P_abi.fire = attr[P_abi.A_ID].fire;
	P_abi.thunder = attr[P_abi.A_ID].thunder;
	P_abi.water = attr[P_abi.A_ID].water;
	P_abi.ice = attr[P_abi.A_ID].ice;

	//�v���C���[�̏��
	g_position = { 0.0f,0.0f,0.0f };
	g_rotation = { 0.0f,0.0f,0.0f };

	g_player.charge_F = 0;
	g_player.charge_N = 0;
	g_player.charge_W = 0;
	g_player.charge_I = 0;
	g_player.charge_T = 0;

	g_player.ULT_F = 0;
	g_player.ULT_N = 0;
	g_player.ULT_W = 0;
	g_player.ULT_I = 0;
	g_player.ULT_T = 0;

	g_player.F_Time = 0;
	g_player.N_Time = 0;
	g_player.W_Time = 0;
	g_player.I_Time = 0;
	g_player.T_Time = 0;
}

// �X�V
void UpdatePlayer() {
	// �������擾
	float height;
	
	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, g_scale, g_rotation, Playertrans);

	// �͈̓`�F�b�N
	if (ReturnSceneNum() == 3) {
		//�g���[�j���O���[��
		if (g_position.x <= -p_troom.TRoomX_MAX / 2 + g_player.playerlimit) {
			g_position.x = -p_troom.TRoomX_MAX / 2 + g_player.playerlimit;
		}
		if (g_position.x >= p_troom.TRoomX_MAX / 2 - g_player.playerlimit) {
			g_position.x = p_troom.TRoomX_MAX / 2 - g_player.playerlimit;
		}
		if (g_position.z >= p_troom.TRoomZ_MAX / 2 - g_player.playerlimit) {
			g_position.z = p_troom.TRoomZ_MAX / 2 - g_player.playerlimit;
		}
		if (g_position.z <= -p_troom.TRoomZ_MAX / 2 + g_player.playerlimit) {
			g_position.z = -p_troom.TRoomZ_MAX / 2 + g_player.playerlimit;
		}
		height = GetTRoomHeight(g_position);
	}
	else {
		//�t�B�[���h
		if (g_position.x <= -p_field.FieldX_MAX / 2 + g_player.playerlimit) {
			g_position.x = -p_field.FieldX_MAX / 2 + g_player.playerlimit;
		}
		if (g_position.x >= p_field.FieldX_MAX / 2 - g_player.playerlimit) {
			g_position.x = p_field.FieldX_MAX / 2 - g_player.playerlimit;
		}
		if (g_position.z >= p_field.FieldZ_MAX / 2 - g_player.playerlimit) {
			g_position.z = p_field.FieldZ_MAX / 2 - g_player.playerlimit;
		}
		if (g_position.z <= -p_field.FieldZ_MAX / 2 + g_player.playerlimit) {
			g_position.z = -p_field.FieldZ_MAX / 2 + g_player.playerlimit;
		}
		height = GetFieldHeight(g_position);
	}
	Weapon_ST* weapon_P = GetWeaponST();

	if (g_player.state != PlayerState::LIVE)return;

	//�U�����ɑO�i
	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;

	zAxis.x = g_position.x + zAxis.x;
	zAxis.z = g_position.z + zAxis.z;
	if (weapon_P->Max_charge == true) {
		g_attackmove = ((zAxis - g_position) / 10) * weapon_P->chargeAtMove;
	}
	else{
		g_attackmove = (zAxis - g_position) / 10;
	}

	if (weapon_P->attTime >= 1&& weapon_P->attTime <= 10){
		//xyz�����ꂩ�������Ă���΃_�b�V�����Ȃ���U��
		if (g_move.x >= g_movechack.x || g_move.y >= g_movechack.y || g_move.z >= g_movechack.z) {
			g_move.x -= g_attackmove.x;
			g_move.z -= g_attackmove.z;
		}
		if (g_move.x <= -g_movechack.x || g_move.y <= -g_movechack.y || g_move.z <= -g_movechack.z) {
			g_move.x -= g_attackmove.x;
			g_move.z -= g_attackmove.z;
		}
	}

	MovePlayer();
	// �ʒu�ړ�
	g_position += g_move;
	g_position.y = height;

	// �ړ��ʂɊ�����������(��������)
	g_move += -g_move * RATE_MOVE_MODEL;
	
	UpdatePlayerBattle();
}
//�ړ�
void MovePlayer() {

	DirectX::XMFLOAT3 rotCamera;			// �J�����̎p�����擾����i�I�C���[�p�j
	// �J�������p�x���擾
	rotCamera = Camera::GetRotCamera();

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_A)) {
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
		{// ���O�ړ�

			float radian;
			radian = rotCamera.y + DirectX::XM_PI * 0.75f;

			g_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			g_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// �ڕW�p�x���Z�b�g
			g_destrot.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// ����ړ�

			float radian;
			radian = rotCamera.y + DirectX::XM_PI * 0.25f;

			g_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			g_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// �ڕW�p�x���Z�b�g
			g_destrot.y = radian;
		}
		else
		{// ���ړ�

			float radian;
			radian = rotCamera.y + DirectX::XM_PI * 0.50f;

			g_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			g_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// �ڕW�p�x���Z�b�g
			g_destrot.y = radian;
		}
	}

	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D))
	{
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W)) {
			// �E�O�ړ�

			float radian;
			radian = rotCamera.y - DirectX::XM_PI * 0.75f;

			g_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			g_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// �ڕW�p�x���Z�b�g
			g_destrot.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// �E��ړ�
			float radian;
			radian = rotCamera.y + -DirectX::XM_PI * 0.25f;

			g_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			g_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// �ڕW�p�x���Z�b�g
			g_destrot.y = radian;
		}
		else
		{// �E�ړ�

			float radian;
			radian = rotCamera.y + -DirectX::XM_PI * 0.50f;

			g_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			g_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// �ڕW�p�x���Z�b�g
			g_destrot.y = radian;
		}
	}
	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
	{
		// �O�ړ�
		g_move.x -= sinf(DirectX::XM_PI + rotCamera.y) * VALUE_MOVE_MODEL;
		g_move.z -= cosf(DirectX::XM_PI + rotCamera.y) * VALUE_MOVE_MODEL;

		// �ڕW�p�x���Z�b�g
		g_destrot.y = rotCamera.y + DirectX::XM_PI;
	}

	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
	{
		// ��ړ�
		g_move.x -= sinf(rotCamera.y) * VALUE_MOVE_MODEL;
		g_move.z -= cosf(rotCamera.y) * VALUE_MOVE_MODEL;

		// �ڕW�p�x���Z�b�g
		g_destrot.y = rotCamera.y;;
	}

	// �ڕW�p�x�ƌ��݊p�x�Ƃ̍��������߂�
	float diffrot = g_destrot.y - g_rotation.y;
	if (diffrot > DirectX::XM_PI)
	{
		diffrot -= DirectX::XM_PI * 2.0f;
	}
	if (diffrot < -DirectX::XM_PI)
	{
		diffrot += DirectX::XM_PI * 2.0f;
	}

	// �p�x���x�Ɋ�����t����
	g_rotation.y += diffrot * RATE_ROTATE_MODEL;

	if (g_rotation.y > DirectX::XM_PI)
	{
		g_rotation.y -= DirectX::XM_PI * 2.0f;
	}
	if (g_rotation.y < -DirectX::XM_PI)
	{
		g_rotation.y += DirectX::XM_PI * 2.0f;
	}
}
//�o�g���̏���
void UpdatePlayerBattle() {

	PlayerAbiChange();
	//�������̕K�E�Z-----------------------------------------------------------------------------
	if (g_player.charge_N >= g_player.NeedCharge && P_abi.A_ID == g_player.NORMAL_ID &&
		CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_Q)) {
		g_player.ULT_N = 1;
		g_player.charge_N = 0;
		PlaySound(SE8);
	}

	if (g_player.ULT_N == 1) {
		NStar.Update();
		g_player.N_Time++;
		if (g_player.N_Time >= g_player.SkillDuration) {
			g_player.ULT_N = 0;
			g_player.N_Time = 0;
		}
	}
	//�Α����̕K�E�Z-----------------------------------------------------------------------------
	if (g_player.charge_F >= g_player.NeedCharge && P_abi.A_ID == g_player.FIRE_ID &&
		CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_Q)) {
		g_player.ULT_F = 1;
		g_player.charge_F = 0;
		PlaySound(SE8);
	}

	if (g_player.ULT_F == 1) {
		fireball.Update();
		g_player.F_Time++;
		if (g_player.F_Time >= g_player.SkillDuration) {
			g_player.ULT_F = 0;
			g_player.F_Time = 0;
		}
	}
	//�������̕K�E�Z-----------------------------------------------------------------------------
	if (g_player.charge_W >= g_player.NeedCharge && P_abi.A_ID == g_player.WATER_ID &&
		CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_Q)) {
		g_player.ULT_W = 1;
		g_player.charge_W = 0;
		PlaySound(SE8);
		PlaySound(SE4);
	}

	if (g_player.ULT_W == 1) {
		g_player.W_Time++;
		if (P_abi.HP < state[P_abi.ID].HP) {
			P_abi.HP += state[P_abi.ID].HP / 500;
		}
		if (g_player.W_Time >= g_player.SkillDuration) {
			StopSound(SE4);
			g_player.ULT_W = 0;
			g_player.W_Time = 0;
		}
	}
	//�X�����̕K�E�Z-----------------------------------------------------------------------------
	if (g_player.charge_I >= g_player.NeedCharge && P_abi.A_ID == g_player.ICE_ID &&
		CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_Q)) {
		g_player.ULT_I = 1;
		g_player.charge_I = 0;
		PlaySound(SE8);
		PlaySound(SE4);
	}

	if (g_player.ULT_I == 1) {
		UpdateSnowParticle();
		g_player.I_Time++;
		if (g_player.I_Time >= g_player.SkillDuration) {
			StopSound(SE4);
			g_player.ULT_I = 0;
			g_player.I_Time = 0;
		}
	}

	//�������̕K�E�Z�i���j-----------------------------------------------------------------------
	if (g_player.charge_T >= g_player.NeedCharge && P_abi.A_ID == g_player.THUNDER_ID &&
		CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_Q)) {
		g_player.ULT_T = 1;
		g_player.charge_T = 0;
		g_player.ThunderSkillChack = true;
		PlaySound(SE8);
		PlaySound(SE3);
	}

	if (g_player.ULT_T == 1) {
		g_player.T_Time++;
		if (g_player.T_Time >= 60.0f) {
			g_player.ULT_T = 0;
			g_player.T_Time = 0;
		}
	}
}

// �`��
void DrawPlayer() {

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

	if (g_player.ULT_F == 1) {
		fireball.Draw();
	}

	if (g_player.ULT_N == 1) {
		NStar.Draw();
	}

	if (g_player.ULT_I == 1) {
		DrawSnowParticle();
	}
}

 //���������
void ExitPlayer() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
	fireball.Exit();
	NStar.Exit();
}


//�U���̑�����ύX
//�X�L���Q�[�W�̃`���[�W
void PlayerAbiChange() {

	clock_t end = timeGetTime();
	
	//�X�L���̃N�[���^�C���v�Z
	if (g_player.P_sec <= g_player.PossibleTime) {
		//�O��X�L����������������t�Z
		g_player.P_sec = (float)(end - g_player.start) / CLOCKS_PER_SEC;
	}
	//�������̕K�E�Z-----------------------------------------------------------------
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_1)) {
		PlaySound(SE9);
		P_abi.A_ID = g_player.NORMAL_ID;
		//
		if (g_player.P_sec < g_player.PossibleTime)return;

		if (g_player.charge_N < 3 && g_player.ULT_N == 0) {
			g_player.charge_N += 1;
			g_player.start = timeGetTime();
			g_player.P_sec = 0;		//�N�[���^�C��������
		}
	}
	//�Α����̕K�E�Z-----------------------------------------------------------------
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_2)) {
		PlaySound(SE9);
		P_abi.A_ID = g_player.FIRE_ID;

		if (g_player.P_sec < g_player.PossibleTime)return;

		if (g_player.charge_F < 3 && g_player.ULT_F == 0) {
			g_player.charge_F += 1;
			g_player.start = timeGetTime();
			g_player.P_sec = 0;		//�N�[���^�C��������
		}
	}
	//�������̕K�E�Z-----------------------------------------------------------------
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_3)) {
		PlaySound(SE9);
		P_abi.A_ID = g_player.WATER_ID;

		if (g_player.P_sec < g_player.PossibleTime)return;

		if (g_player.charge_W < 3 && g_player.ULT_W == 0) {
			g_player.charge_W += 1;
			g_player.start = timeGetTime();
			g_player.P_sec = 0;		//�N�[���^�C��������
		}
	}
	//�X�����̕K�E�Z-----------------------------------------------------------------
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_4)) {
		PlaySound(SE9);
		P_abi.A_ID = g_player.ICE_ID;

		if (g_player.P_sec < g_player.PossibleTime)return;

		if (g_player.charge_I < 3 && g_player.ULT_I == 0) {
			g_player.charge_I += 1;
			g_player.start = timeGetTime();
			g_player.P_sec = 0;		//�N�[���^�C��������
		}
	}
	//�������̕K�E�Z-----------------------------------------------------------------
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_5)) {
		PlaySound(SE9);
		P_abi.A_ID = g_player.THUNDER_ID;

		if (g_player.P_sec < g_player.PossibleTime)return;

		if (g_player.charge_T < 3 && g_player.ULT_T == 0) {
			g_player.charge_T += 1;
			g_player.start = timeGetTime();
			g_player.P_sec = 0;		//�N�[���^�C��������
		}
	}
	//-------------------------------------------------------------------------------
}