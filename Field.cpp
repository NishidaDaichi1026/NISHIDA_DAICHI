#include	"Field.h"
#include	"Player.h"
#include	"CPlane.h"
#include	"Collision.h"

using namespace DirectX::SimpleMath;

// ���p
static CShader		g_shaderfield;
//  �����b�V��
static CUndulationPlaneMesh g_planemesh;
// �����b�V���i�����_�����O�f�[�^�j
static CMeshRenderer g_meshrenderer;
// ���p�}�e���A��
static CMaterial	g_material;
// SRV
static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_srv;

std::vector<CPlane> g_planes;

Field g_field;

// �v���C���̏��(S,R,T)---------------------------------------------------------------

static DirectX::XMFLOAT3	g_position = { 0.0f,0.0f,0.0f };		// �ʒu
static DirectX::XMFLOAT3	g_rotation = { 0.0f,0.0f,0.0f };		// �p��
static DirectX::XMFLOAT3	g_scale = { 1.0f,1.0f,1.0f };			// �g�k

//-------------------------------------------------------------------------------------


void makepalenequatation() {

	for (unsigned int idx = 0; idx < g_planemesh.GetIndices().size()/ 3; idx++) {
		CPlaneMesh::FACE f = g_planemesh.GetTriangle(idx);

		VERTEX_3D v0 = g_planemesh.GetVertices()[f.idx[0]];
		VERTEX_3D v1 = g_planemesh.GetVertices()[f.idx[1]];
		VERTEX_3D v2 = g_planemesh.GetVertices()[f.idx[2]];

		CPlane p;
		p.MakeEquatation(v0.Position,v1.Position,v2.Position);
		g_planes.emplace_back(p);
	}
}

// ���݈ʒu�̍��������߂�
float  GetFieldHeight(Vector3 pos) {
	static bool firstflag = true;
	if (firstflag == true) {
		makepalenequatation();
		firstflag = false;
	}

	float t;
	// �ʐ���
	for (unsigned int idx = 0; idx < g_planes.size(); idx++) {
		Vector3 up = {0,1,0};
		Vector3 startpoint = {pos.x,0,pos.z};
		Plane p = g_planes[idx].GetPlaneInfo().plane;
		Vector3 ans;

		bool sts = LinetoPlaneCross(p, startpoint,up, t, ans);
		if (sts) {
			sts = CheckInTriangle(
				g_planes[idx].GetPlaneInfo().p0,
				g_planes[idx].GetPlaneInfo().p1,
				g_planes[idx].GetPlaneInfo().p2, ans);
			if (sts) return ans.y;
		}
	}

	return 0;
}

// ������
void InitField() {
	// �g�p����V�F�[�_�[�𐶐�
	g_shaderfield.SetShader(
		"shader/unlitTextureVS.hlsl",				// ���_�V�F�[�_
		"shader/unlitTexturePS.hlsl");				// �s�N�Z���V�F�[�_

	// �����b�V������
	g_planemesh.Init(
		10, 10,					// ������
		g_field.FieldX_MAX,		// �T�C�Y
		g_field.FieldZ_MAX,		// �T�C�Y
		Color(1, 1, 1, 1),		// ���_�J���[
		Vector3(0, 0, -1),		// �@���x�N�g��z
		true);					// XZ����

	// �ʉ���t����i��l���z�Łj
	g_planemesh.MakeUndulation(
		0.0f,					// �ŏ�
		10.0f);					// �ő�

	// ���b�V�������_�������i���p�j
	g_meshrenderer.Init(g_planemesh);

	ID3D11Device* dev;
	ID3D11DeviceContext* devcontext;

	dev = Renderer::GetDevice();
	devcontext = Renderer::GetDeviceContext();

	bool sts = CreateSRVfromFile("assets/texture/field000.jpg", dev, devcontext, &g_srv);
	if (!sts) {
		MessageBox(nullptr, "CreateSRVfromFile error(MeshWallInit)", "error", MB_OK);
	}

	// �}�e���A��������
	MATERIAL mtrl;
	mtrl.Ambient = Color(1, 1, 1, 1);
	mtrl.Diffuse = Color(1, 1, 1, 1);
	mtrl.Emission = Color(0, 0, 0, 0);
	mtrl.Specular = Color(0, 0, 0, 0);
	mtrl.Shininess = 0;
	mtrl.TextureEnable = TRUE;

	g_material.Init(mtrl);

}

// �t�B�[���h�`��
void DrawField() {

	// ���p�V�F�[�_�Z�b�g
	g_shaderfield.SetGPU();

	// ���p�}�e���A���Z�b�g
	g_material.SetGPU();

	// �e�N�X�`�����Z�b�g
	ID3D11DeviceContext* devcontext;
	devcontext = Renderer::GetDeviceContext();
	devcontext->PSSetShaderResources(0, 1, g_srv.GetAddressOf());

	// GPU�ɍs����Z�b�g����
	Matrix mtx;
	DX11MakeWorldMatrix(
		mtx,
		g_scale,							// �X�P�[��
		g_rotation,							// �p��
		g_position);						// �ʒu

	Renderer::SetWorldMatrix(&mtx);

	// ���`��
	g_meshrenderer.Draw();
}
