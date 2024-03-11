#include	"FireBall.h"
#include	"Player.h"
//------------------------------------------------
//			���̕K�E�Z�̃��f��
//------------------------------------------------

using namespace DirectX::SimpleMath;

// �`��ׂ̈ɕK�v�ȏ��
// �g�p����V�F�[�_�[
static CShader		g_shader;

// �X�^�e�B�b�N���b�V���i�W�I���g���f�[�^�j
static CStaticMesh g_staticmesh;

// ���b�V�������_���[
static CStaticMeshRenderer g_staticmeshrenderer;

// Bounding sphere
static CBoundingSphere g_bs;
// ��
static CSphereMesh g_sphere;
// ���b�V�������_��
static CMeshRenderer g_meshrenderer;
// �}�e���A��
static CMaterial g_material;

DirectX::XMFLOAT3 poTrans(0, 0, 0);

CFireBall g_fireball;

//��]
static float rot_sp = 0.0f;


CFireBall* GetFireBall(){
	return &g_fireball;
}

BoundingSphere GetFireBallBS() {

	// ���[���h�ϊ��s�񐶐�
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_fireball.scale,							// �X�P�[��
		g_fireball.rotation,						// �p��
		g_fireball.position);						// �ʒu

	// BS����
	BoundingSphere bs;
	bs = g_bs.MakeBS(mtx, g_fireball.scale);

	return bs;
}

void CFireBall::Init()
{
	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// ���_�V�F�[�_
		"shader/vertexLightingPS.hlsl");				// �s�N�Z���V�F�[�_

	// ���f���t�@�C����
	std::string filename[] = {
		u8"assets\\model\\fireball.obj",
	};

	// ���b�V�������i�W�I���g���f�[�^�j
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

void CFireBall::Exit(){
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}

void CFireBall::Update() {

	rot_sp += 0.05f;		//��]
	Player* player = GetPlayer();
	
	g_fireball.position = GetPositionPlayer();

	g_fireball.rotation.y = rot_sp;

	g_fireball.move = GetMovePlayer();				// �v���C���̈ړ��ʎ擾

	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, g_fireball.scale, g_fireball.rotation, poTrans);

	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;
	//�ʒu����
	g_fireball.position = g_fireball.position + zAxis;
	g_fireball.position.y = g_fireball.position.y + 7.0f;

}

void CFireBall::Draw()
{
	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// ���[���h�ϊ��s�񐶐�
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_fireball.scale,							// �X�P�[��
		g_fireball.rotation,						// �p��
		g_fireball.position);						// �ʒu

	// GPU�ɍs����Z�b�g����
	Renderer::SetWorldMatrix(&mtx);

	// �V�F�[�_�[��GPU�ɃZ�b�g
	g_shader.SetGPU();

	// ���f���`��
	g_staticmeshrenderer.Draw();

}