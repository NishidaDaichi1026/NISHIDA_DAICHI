#include	<random>
#include	"Slime.h"
#include	"CSlime.h"
#include	"CSphereMesh.h"
#include	"renderer.h"
#include	"CBoundingSphere.h"
#include	"Field.h"


using namespace DirectX::SimpleMath;
//�G�̍ő吔
constexpr unsigned int SLIME_MAX = 10;
// �G�l�~�[�Q
static std::vector<CSLIME>	g_Slimes;

static std::mt19937		g_rnd;				// �����Z���k�c�C�X�^�[

Field e_field;

//----------------------------------------------------------------------------
// ���擾

// �ʒu�擾
DirectX::XMFLOAT3 GetPositionSlime(int idx) {
	return g_Slimes[idx].GetPosition();
}
// �p���擾
DirectX::XMFLOAT3 GetRotationSlime(int idx) {
	return g_Slimes[idx].GetRotation();
}
// �ړ��ʎ擾
DirectX::XMFLOAT3 GetMoveSlime(int idx) {
	return g_Slimes[idx].GetMove();
}
//�G�̍ő吔�擾
int SlimeMax() {
	return SLIME_MAX;
}
//----------------------------------------------------------------------------

// ������
void InitSlime()
{
	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// ���_�V�F�[�_
		"shader/vertexLightingPS.hlsl");				// �s�N�Z���V�F�[�_

	// ���f���t�@�C����
	std::string filename[] = {
		u8"assets\\model\\�}�O�}��\\magma_rock.obj",
		u8"assets\\model\\�X�X���C��\\�X�X���C��.obj"
	};

	// ���b�V�������i�W�I���g���f�[�^�j
	g_staticmesh.Init(filename[1]);

	// �`��ׂ̈̃f�[�^����
	g_staticmeshrenderer.Init(g_staticmesh);

	//// �G�l�~�[���T�C�Y
	g_Slimes.resize(SLIME_MAX);

	InitSlimeState();
	
	// ������
	for (auto& s : g_Slimes) {
		s.SetShader(&g_shader);
		s.SetMesh(&g_staticmesh);
		s.SetMeshRenderer(&g_staticmeshrenderer);
	}

	MATERIAL mtrl;
	mtrl.Ambient = Color(0, 0, 0, 0);
	mtrl.Diffuse = Color(1, 1, 1, 0.3f);
	mtrl.Specular = Color(0, 0, 0, 0);
	mtrl.Shininess = 0;
	mtrl.Emission = Color(0, 0, 0, 0);
	mtrl.TextureEnable = FALSE;

	g_material.Init(mtrl);

	// ���E�������߂�
	g_bs.Caliculate(g_staticmesh.GetVertices());

	// �����b�V���𐶐�����
	g_sphere.Init(g_bs.GetRadius(), Color(1, 1, 1, 1), 100, 100);
	g_meshrenderer.Init(g_sphere);
}

//�X�e�[�^�X������
void InitSlimeState() {

	std::random_device rnddev;			// �^�̗���

	g_rnd.seed(rnddev());				// �V�[�h�l�Z�b�g

	// �������z��
	std::uniform_real_distribution<float>	distx(-e_field.FieldX_MAX / 3, e_field.FieldX_MAX / 3);
	std::uniform_real_distribution<float>	distz(-e_field.FieldZ_MAX / 3, e_field.FieldZ_MAX / 3);
	std::uniform_real_distribution<float>	disty(0.0f, 0.0f);


	for (auto& s : g_Slimes) {
		Vector3 Pos{};
		Pos.x = distx(g_rnd);
		Pos.y = disty(g_rnd);
		Pos.z = distz(g_rnd);
		s.InitSlime_ST();
		s.SetPosition(Pos);
	}
}

// �X�V
void UpdateSlime()
{
	for (auto& s : g_Slimes) {
		s.Update();
	}
	
}

// �`��
void DrawSlime()
{
	// �G�`��
	for (auto& s : g_Slimes) {
		s.Draw();
	}
}

// �I������
void ExitSlime() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}