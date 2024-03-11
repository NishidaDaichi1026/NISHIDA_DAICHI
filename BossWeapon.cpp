#include	"BossWeapon.h"
#include	"BossEnemy.h"

#include	"Player.h"
#include	"XAudio2.h"

using namespace DirectX::SimpleMath;

// �v���C���̏��(S,R,T)
static Vector3	g_position = { 0.0f,0.0f,200.0f };		// �ʒu
static Vector3	g_rotation = { 0.0f,0.0f,0.0f };		// �p��
static Vector3	g_scale = { 0.2f,0.2f,0.4f };			// �g�k

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
DirectX::XMFLOAT3 bwTrans(0, 0, 0);


DirectX::SimpleMath::Matrix		g_mtx;

BossWeapon_ST bossweapon_st;

//�{�X�̕���̏��擾-------------------------------------
BossWeapon_ST* GetBossWeaponST() {
	return &bossweapon_st;
}
DirectX::SimpleMath::Vector3 GetBWPosition() {
	return g_position;
}
DirectX::SimpleMath::Vector3 GetBWScale() {
	return g_scale;
}
DirectX::SimpleMath::Vector3 GetBWMove() {
	return g_move;
}
DirectX::SimpleMath::Vector3 GetBWRotation() {
	return g_rotation;;
}
DirectX::SimpleMath::Matrix GetBWMatrix() {
	return g_mtx;
}
//---------------------------------------------------------
BoundingSphere GetBossWeaponBS() {

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

void InitBossWeapon()
{
	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// ���_�V�F�[�_
		"shader/vertexLightingPS.hlsl");				// �s�N�Z���V�F�[�_

	// ���f���t�@�C����
	std::string filename[] = {
		u8"assets\\model\\ax.fbx",
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

void ExitBossWeapon() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}

//�ȉ��{�X�̍U���Ɏg�p-------------------------------------
void UpdateBossWeapon() {

	g_position = GetBossPosition();

	g_rotation = GetBossRotation();
	
	g_move = GetBossMove();

	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, g_scale, g_rotation, bwTrans);

	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;

	g_position = g_position + zAxis * 5.0f;
	g_position.y = g_position.y;

	Vector3 rAxis;
	rAxis.x = mtx._21;
	rAxis.y = mtx._22;
	rAxis.z = mtx._23;

	g_rotation = g_rotation + rAxis;

	BoundingSphere pbs = GetPlayerBS();
	Boss_ST* b_st = GetBossST();
	
	//�{�X�̍U���q�b�g����
	bool sts = HitCheckSphere(pbs, GetBossWeaponBS());

	//��苗�����Ƀv���C���[������΍U���t���O�𗧂Ă�
	//�]�T���������邽�߁{�P�O
	if (b_st->FromPlayerLength <= b_st->S_Int_Leangth + 10) {
		//�O�̍U�������莞�ԋ󂢂Ă����
		if (bossweapon_st.BossAIChack >= bossweapon_st.BossAttInterval) {
			PlaySound(SE1);
			bossweapon_st.attcheck = 1;
			bossweapon_st.WeaponHitCheck = 1;
		}
	}
	//�U�����q�b�g������_���[�W�v�Z
	if (sts && bossweapon_st.WeaponHitCheck == 1) {
		BossDamageCalc();
		PlaySound(SE7);
		bossweapon_st.WeaponHitCheck = 0;
	}
	//�U���A�j���[�V����
	if (bossweapon_st.attcheck == 1) {
		g_position = g_position - zAxis * 10.0f;
		g_position.y = g_position.y + 10.0f;

		g_rotation.x -= (bossweapon_st.attTime - 20) * 0.05f;
		g_rotation.y -= (bossweapon_st.attTime - 20) * 0.05f;

		bossweapon_st.attTime++;
	}
	//�U���A�j���[�V�����I��
	if (bossweapon_st.attTime >= bossweapon_st.AttAnimeTime) {
		bossweapon_st.attcheck = 0;
		bossweapon_st.attTime = 0;
	}

	if (bossweapon_st.attcheck == 0) {		//�U���̃A�j���[�V���������Ă��Ȃ����
		bossweapon_st.BossAIChack++;		//�U���Ԋu�����
	}
	else
	{
		bossweapon_st.BossAIChack = 0;
	}
}

void BossDamageCalc() {
	
	BossAbility* b_abi = GetBossABI();
	PlayerAbility* p_abi = GetPlayerAbi();

	b_abi->DAMAGE = b_abi->ATTACK - p_abi->DEFENCE;
	p_abi->HP = p_abi->HP - b_abi->DAMAGE;

}
//---------------------------------------------------------
void DrawBossWeapon()
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