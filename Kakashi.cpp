#include	"CSphereMesh.h"
#include	"renderer.h"
#include	"CBoundingSphere.h"
#include	"Field.h"
#include	"CKakashi.h"
#include	"Kakashi.h"

using namespace DirectX::SimpleMath;
//�G�̍ő吔
constexpr unsigned int Kakashi_MAX = 5;
// �G�l�~�[�Q
static std::vector<CKAKASHI>	g_kakashi;

static std::mt19937		g_rnd;				// �����Z���k�c�C�X�^�[

Field k_field;

// ������
void InitKakashi()
{
	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// ���_�V�F�[�_
		"shader/vertexLightingPS.hlsl");				// �s�N�Z���V�F�[�_

	// ���f���t�@�C����
	std::string filename[] = {
		u8"assets\\model\\kakashi.fbx",
	};

	// ���b�V�������i�W�I���g���f�[�^�j
	g_staticmesh.Init(filename[0]);

	// �`��ׂ̈̃f�[�^����
	g_staticmeshrenderer.Init(g_staticmesh);

	//// �G�l�~�[���T�C�Y
	g_kakashi.resize(Kakashi_MAX);

	InitKakashiState();

	// ������
	for (auto& m : g_kakashi) {
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
void InitKakashiState() {
	int i = 0;
	for (auto& m : g_kakashi) {
		Vector3 Pos{};
		Pos.x = (i * 75)-150;
		Pos.y = 0;
		Pos.z = 0;
		m.SetA_ID(i);
		m.InitKakashi_ST();
		m.SetPosition(Pos);
		i++;
	}
}

// �X�V
void UpdateKakashi()
{
	for (auto& m : g_kakashi) {
		m.Update();
	}

}

// �`��
void DrawKakashi()
{
	// �G�`��
	for (auto& m : g_kakashi) {
		m.Draw();
	}
}

// �I������
void ExitKakashi() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}