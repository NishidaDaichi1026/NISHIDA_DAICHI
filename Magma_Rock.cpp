#include	<random>
#include	"Magma_Rock.h"
#include	"CMagma_Rock.h"
#include	"CSphereMesh.h"
#include	"renderer.h"
#include	"CBoundingSphere.h"
#include	"Field.h"

using namespace DirectX::SimpleMath;
//�G�̍ő吔
constexpr unsigned int Magma_MAX = 10;
// �G�l�~�[�Q
static std::vector<CMagma>	g_MRocks;

static std::mt19937		g_rnd;				// �����Z���k�c�C�X�^�[

Field m_field;

//�G�̏��擾-----------------------------------------------
// �ʒu�擾
DirectX::XMFLOAT3 GetPositionMagma(int idx) {
	return g_MRocks[idx].GetPosition();
}
// �p���擾
DirectX::XMFLOAT3 GetRotationMagma(int idx) {
	return g_MRocks[idx].GetRotation();
}
// �ړ��ʎ擾
DirectX::XMFLOAT3 GetMoveMagma(int idx) {
	return g_MRocks[idx].GetMove();
}
//�@�G�̍ő吔�擾
int MagmaMax() {
	return Magma_MAX;
}
//-----------------------------------------------------------

// ������
void InitMagma()
{
	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// ���_�V�F�[�_
		"shader/vertexLightingPS.hlsl");				// �s�N�Z���V�F�[�_

	// ���f���t�@�C����
	std::string filename[] = {
		u8"assets\\model\\�}�O�}��\\magma_rock.obj",
	};

	// ���b�V�������i�W�I���g���f�[�^�j
	g_staticmesh.Init(filename[0]);

	// �`��ׂ̈̃f�[�^����
	g_staticmeshrenderer.Init(g_staticmesh);

	//// �G�l�~�[���T�C�Y
	g_MRocks.resize(Magma_MAX);

	InitMagmaState();

	// ������
	for (auto& m : g_MRocks) {
		m.SetShader(&g_shader);
		m.SetMesh(&g_staticmesh);
		m.SetMeshRenderer(&g_staticmeshrenderer);
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
void InitMagmaState() {

	std::random_device rnddev;			// �^�̗���

	g_rnd.seed(rnddev());				// �V�[�h�l�Z�b�g

	// �������z��
	std::uniform_real_distribution<float>	distx(-m_field.FieldX_MAX / 3, m_field.FieldX_MAX / 3);
	std::uniform_real_distribution<float>	distz(-m_field.FieldZ_MAX / 3, m_field.FieldZ_MAX / 3);
	std::uniform_real_distribution<float>	disty(0.0f, 0.0f);

	for (auto& m : g_MRocks) {
		Vector3 Pos{};
		Pos.x = distx(g_rnd);
		Pos.y = disty(g_rnd);
		Pos.z = distz(g_rnd);
		m.InitMagma_ST();
		m.SetPosition(Pos);
	}
}

// �X�V
void UpdateMagma()
{
	for (auto& m : g_MRocks) {
		m.Update();
	}

}

// �`��
void DrawMagma()
{
	// �G�`��
	for (auto& m : g_MRocks) {
		m.Draw();
	}
}

// �I������
void ExitMagma() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}