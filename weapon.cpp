#include	"weapon.h"
#include	"Player.h"
#include	"XAudio2.h"

using namespace DirectX::SimpleMath;

// �v���C���̏��(S,R,T)-------------------------------------------------------
static Vector3	g_position = { 0.0f,0.0f,0.0f };		// �ʒu
static Vector3	g_rotation = { 0.0f,0.0f,0.0f };		// �p��
static Vector3	g_scale = { 2.0f,2.0f,2.0f };			// �g�k
static Vector3	g_move = { 0.0f,0.0f,0.0f };			// �ړ���
static Vector3	g_destrot = { 0.0f,0.0f,0.0f };			// �ڕW��]�p�x
DirectX::XMFLOAT3 wTrans(0, 0, 0);
//-----------------------------------------------------------------------------
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

Weapon_ST weapon_st;
HitStop h_stop;
//--------------------------------------------------------
//���擾
Weapon_ST* GetWeaponST() {
	return &weapon_st;
}

HitStop* GetHitStop() {
	return &h_stop;
}

//�ʒu�擾
DirectX::XMFLOAT3 GetPositionWeapon() {
	return g_position;
}

BoundingSphere GetWeaponBS() {

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
//--------------------------------------------------------

//������
void InitWeapon()
{
	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// ���_�V�F�[�_
		"shader/vertexLightingPS.hlsl");				// �s�N�Z���V�F�[�_

	// ���f���t�@�C����
	std::string filename[] = {
		u8"assets\\model\\sowrd.obj",
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

}
//���������
void ExitWeapon() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}
//�X�V
void UpdateWeapon() {

	g_position = GetPositionPlayer();

	g_rotation = GetRotationPlayer();				// �v���C���̌����擾

	g_move = GetMovePlayer();				// �v���C���̈ړ��ʎ擾

	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, g_scale, g_rotation, wTrans);

	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;
	
	g_position = g_position + zAxis * 4.0f;
	g_position.y = g_position.y + 5.0f;

	Vector3 rAxis;
	rAxis.x = mtx._21;
	rAxis.y = mtx._22;
	rAxis.z = mtx._23;

	g_rotation = g_rotation + rAxis;
	
	//�U���A�j���[�V����--------------------------------------------------------

	if (CDirectInput::GetInstance().GetMouseRButtonCheck() &&
		weapon_st.WeaponHitCheck != 1) {
		//�`���[�W�ϐ�
		if (weapon_st.charge <= weapon_st.chargeTime) {
			weapon_st.charge++;
		}
		//�`���[�W���̈ʒu
		g_position = g_position - zAxis * 4.0f;
		g_position.y = g_position.y + 7.0f;
		g_rotation.x = 300.0f;

		if (CDirectInput::GetInstance().GetMouseLButtonTrigger() ||
			CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_E) &&
			weapon_st.charge >= weapon_st.chargeTime
			&& weapon_st.attTime == 0) {

			weapon_st.Max_charge = true;
			weapon_st.WeaponHitCheck = 1;
			PlaySound(SE1);
		}
	}
	else{
		if (CDirectInput::GetInstance().GetMouseLButtonTrigger() ||
			CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_E) &&
			weapon_st.attTime == 0) {
			weapon_st.WeaponHitCheck = 1;
			PlaySound(SE1);
		}
		weapon_st.charge = 0.0f;
	}

	if (weapon_st.WeaponHitCheck == 1) {
		g_position = g_position - zAxis * 4.0f;
		g_position.y = g_position.y + 10.0f;

		g_rotation.x -= weapon_st.attTime * 0.1f;
		g_rotation.y -= weapon_st.attTime * 0.1f;
		
		weapon_st.attTime++;
	}
	//��莞�Ԍo�߂ŏI��
	if (weapon_st.attTime >= weapon_st.attackCoolTime) {
		weapon_st.attTime = 0;
		weapon_st.WeaponHitCheck = 0;
		weapon_st.charge = 0.0f;
		weapon_st.Max_charge = false;
	}
	//--------------------------------------------------------------------------
}
//�`��
void DrawWeapon()
{
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

}