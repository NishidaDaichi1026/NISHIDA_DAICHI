#include	<d3d11.h>
#include	<SimpleMath.h>
#include	<WICTextureLoader.h>

#include	"game.h"
#include	"CShader.h"
#include	"dx11helper.h"
#include	"camera.h"
#include	"SummarizeEnemy.h"
#include	"Player.h"
#include	"sceneChange.h"
#include	"CDirectInput.h"
#include	"CMaterial.h"
#include	"TitleCom.h"
#include	"TitleImg.h"

using namespace DirectX::SimpleMath;

// �g�p����V�F�[�_�[
static CShader g_shader;

// RTS���---------------------------------------------------------------------------------------------

static DirectX::SimpleMath::Vector3	g_Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
static DirectX::SimpleMath::Vector3	g_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
static DirectX::SimpleMath::Vector3	g_Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

//-----------------------------------------------------------------------------------------------------

// ���_�o�b�t�@
ID3D11Buffer* g_VertexBuffer = nullptr;
// �e�N�X�`�����\�[�X
ID3D11ShaderResourceView* g_Texture = nullptr;
// �}�e���A��
static CMaterial	g_material{};

bool InitTitleScene() {

	float width = SCREEN_WIDTH;
	float height = SCREEN_HEIGHT;

	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/unlitTextureVS.hlsl",				// ���_�V�F�[�_
		"shader/unlitTexturePS.hlsl");				// �s�N�Z���V�F�[�_

	// �e�N�X�`���ǂݍ���
	DirectX::CreateWICTextureFromFile(
		Renderer::GetDevice(),
		L"assets/texture/��116-1536x864.jpg",
		nullptr,
		&g_Texture);

	assert(g_Texture);

	// ���_�o�b�t�@����
	VERTEX_3D vertex[4];

	vertex[0].Position = Vector3(0.0f, 0.0f, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = Vector2(0.0f, 0.0f);

	vertex[1].Position = Vector3(width, 0.0f, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = Vector2(1.0f, 0.0f);

	vertex[2].Position = Vector3(0.0f, height, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = Vector2(0.0f, 1.0f);

	vertex[3].Position = Vector3(width, height, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = Vector2(1.0f, 1.0f);

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &g_VertexBuffer);

	// �}�e���A��������
	MATERIAL mtrl;
	mtrl.Ambient = Color(1, 1, 1, 1);
	mtrl.Diffuse = Color(1, 1, 1, 1);
	mtrl.Emission = Color(0, 0, 0, 0);
	mtrl.Specular = Color(0, 0, 0, 0);
	mtrl.Shininess = 0;
	mtrl.TextureEnable = TRUE;

	g_material.Init(mtrl);

	//�^�C�g���摜
	InitTitleCom();
	InitTitleImg();

	return true;
}

void UpdateTitleScene() {
	//�^�C�g���摜
	UpdateTitleCom();
}

void DrawTitleScene() {

	// 2D�`��p�̃J�����������s
	Camera::Draw2D();

	// �V�F�[�_�[��GPU�ɃZ�b�g
	g_shader.SetGPU();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�e���A���ݒ�
	g_material.SetGPU();

	// �e�N�X�`���ݒ�
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

	// �v���~�e�B�u�g�|���W�ݒ�
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// �|���S���`��
	Renderer::GetDeviceContext()->Draw(4, 0);
	
	//�^�C�g���摜
	DrawTitleCom();
	DrawTitleImg();
	DrawTitleLogo();
	DrawInstOp();
}

void ExitTitleScene() {
	g_VertexBuffer->Release();
	g_material.Uninit();
	g_Texture->Release();
	//�^�C�g���摜
	ExitTitleCom();
	ExitTitleImg();
}