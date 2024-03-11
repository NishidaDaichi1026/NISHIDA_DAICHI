#include	"NormalStar.h"
#include	"Player.h"
//---------------------------------------------
//			�������̕K�E�Z�̃��f��
//---------------------------------------------

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

DirectX::XMFLOAT3 nsTrans(0, 0, 0);

CNormalStar g_normal_star;

static float rot_sp = 0.0f;				//��]���x

CNormalStar* GetNormalStar() {
	return &g_normal_star;
}

BoundingSphere GetNormalStarBS() {

	// ���[���h�ϊ��s�񐶐�
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_normal_star.scale,							// �X�P�[��
		g_normal_star.rotation,						// �p��
		g_normal_star.position);						// �ʒu

	// BS����
	BoundingSphere bs;
	bs = g_bs.MakeBS(mtx, g_normal_star.scale);

	return bs;
}
//������
void CNormalStar::Init()
{
	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// ���_�V�F�[�_
		"shader/vertexLightingPS.hlsl");				// �s�N�Z���V�F�[�_

	// ���f���t�@�C����
	std::string filename[] = {
		u8"assets\\model\\HOSHI.obj",
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
//���������
void CNormalStar::Exit() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}
//�X�V
void CNormalStar::Update() {
	//��]
	rot_sp += 0.005f;

	g_normal_star.position = GetPositionPlayer();

	g_normal_star.rotation.y = rot_sp;

	g_normal_star.move = GetMovePlayer();				// �v���C���̈ړ��ʎ擾

	Player* player = GetPlayer();

	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, g_normal_star.scale, g_normal_star.rotation, nsTrans);

	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;

	g_normal_star.position = g_normal_star.position + zAxis;

	//-----------------------------------------------------------------------------------------------
	//�傫������A�j���[�V����
	if (player->N_Time <= 120) {
		g_normal_star.scale.x = g_normal_star.scale.y = g_normal_star.scale.z += 0.005f;
	}
	else {
		if (g_normal_star.scale.x <= 30.0f) {
			g_normal_star.scale.x = g_normal_star.scale.y = g_normal_star.scale.z += 0.5f;
		}
	}
	//�I�������ɖ߂�
	if (player->N_Time == 0) {
		g_normal_star.scale.x = g_normal_star.scale.y = g_normal_star.scale.z = 0.0f;
	}
	//-----------------------------------------------------------------------------------------------
}
//�`��
void CNormalStar::Draw(){

	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// ���[���h�ϊ��s�񐶐�
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_normal_star.scale,							// �X�P�[��
		g_normal_star.rotation,							// �p��
		g_normal_star.position);						// �ʒu

	// GPU�ɍs����Z�b�g����
	Renderer::SetWorldMatrix(&mtx);

	// �V�F�[�_�[��GPU�ɃZ�b�g
	g_shader.SetGPU();

	// ���f���`��
	g_staticmeshrenderer.Draw();
	
}