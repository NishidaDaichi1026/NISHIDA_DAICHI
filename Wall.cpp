#include	<SimpleMath.h>
#include	<wrl/client.h>
#include	"renderer.h"
#include	"dx11helper.h"
#include	"CShader.h"
#include	"CMaterial.h"
#include	"CPlaneMesh.h"
#include	"CMeshRenderer.h"
#include	"dx11mathutil.h"

using namespace DirectX::SimpleMath;

//*****************************************************************************
// �}�N����`
//*****************************************************************************
constexpr	int MAX_MESH_WALL = 4;									// �ǂ̑���

static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_srv;
static Microsoft::WRL::ComPtr<ID3D11Resource> g_texres;

static Vector3 g_scale = { 1,1,1 };

static float wallSize = 1024.0f;	//�T�C�Y

static Vector3 g_position[MAX_MESH_WALL] = {
	{0,0,wallSize / 2},
	{-wallSize / 2,0,0},
	{wallSize / 2,0,0},
	{0,0,-wallSize / 2}
};

static Vector3 g_rotation[MAX_MESH_WALL] = {
	{0,0,180},
	{180,90.0f,0},
	{180,-90.0f,0},
	{0,180.0f,180}
};

// �Ǘp
static CShader		g_shaderwall;
//  �ǃ��b�V���i�W�I���g���f�[�^�j
static CPlaneMesh g_wallmesh;
// �ǃ��b�V���i�����_�����O�f�[�^�j
static CMeshRenderer g_meshrenderer;
// �Ǘp�}�e���A��
static CMaterial	g_material;

// ������
void InitWall() {

	ID3D11Device* dev;
	ID3D11DeviceContext* devcontext;

	dev = Renderer::GetDevice();
	devcontext = Renderer::GetDeviceContext();

	bool sts = CreateSRVfromFile("assets/texture/wall000.jpg", dev, devcontext, &g_srv);
	if (!sts) {
		MessageBox(nullptr, "CreateSRVfromFile error(MeshWallInit)", "error", MB_OK);
	}

	// �g�p����V�F�[�_�[�𐶐�
	g_shaderwall.SetShader(
		"shader/unlitTextureVS.hlsl",				// ���_�V�F�[�_
		"shader/unlitTexturePS.hlsl");				// �s�N�Z���V�F�[�_

	// �ǃ��b�V������
	g_wallmesh.Init(
		10, 10,					// ������
		wallSize,				// �T�C�Y
		wallSize,				// �T�C�Y
		Color(1, 1, 1, 1),		// ���_�J���[
		Vector3(0, 0, 1));		// �@���x�N�g��

	// ���b�V�������_�������i�Ǘp�j
	g_meshrenderer.Init(g_wallmesh);

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

// �Ǖ`��
void DrawWall() {

	ID3D11Device* dev;
	ID3D11DeviceContext* devcontext;

	dev = Renderer::GetDevice();
	devcontext = Renderer::GetDeviceContext();

	g_shaderwall.SetGPU();

	g_material.SetGPU();

	devcontext->PSSetShaderResources(0, 1, g_srv.GetAddressOf());

	for (int i = 0; i < 4; i++) {

		// �ǂȂ̂�X����]90�x
		// ���[���h�ϊ��s�񐶐�
		Matrix mtx;
		DX11MakeWorldMatrix(
			mtx,
			g_scale,							// �X�P�[��
			g_rotation[i],						// �p��
			g_position[i]);						// �ʒu

		// GPU�ɍs����Z�b�g����
		Renderer::SetWorldMatrix(&mtx);

		g_meshrenderer.Draw();
	}
}