#include	<d3d11.h>
#include	<SimpleMath.h>
#include	<WICTextureLoader.h>
#include	"CDirectInput.h"
#include	"CShader.h"
#include	"dx11helper.h"
#include	"camera.h"
#include	"CMaterial.h"
#include	"Timer.h"
#include	"Player.h"
#include	"SceneFade.h"

using namespace DirectX::SimpleMath;

// �g�p����V�F�[�_�[
static CShader g_shader;

// RTS���
static DirectX::SimpleMath::Vector3	g_Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
static DirectX::SimpleMath::Vector3	g_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
static DirectX::SimpleMath::Vector3	g_Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

// ���_�o�b�t�@
static ID3D11Buffer* g_VertexBuffer{};

// �e�N�X�`�����\�[�X
static ID3D11ShaderResourceView* g_Texture{};

// �}�e���A��
static CMaterial	g_material{};

Timer g_timer;

Timer* GetTimer() {
	return &g_timer;
}

void InitTimer() {

	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/unlitTextureVS.hlsl",				// ���_�V�F�[�_
		"shader/unlitTexturePS.hlsl");				// �s�N�Z���V�F�[�_

	// �e�N�X�`���ǂݍ���
	DirectX::CreateWICTextureFromFile(
		Renderer::GetDevice(),
		L"assets/texture/NUMBER.png",
		nullptr,
		&g_Texture);

	assert(g_Texture);

	// ���_�o�b�t�@����
	VERTEX_3D vertex[4];

	vertex[0].Position = Vector3(0.0f, 0.0f, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = Vector2(0.0f, 0.0f);

	vertex[1].Position = Vector3(200.0f, 0.0f, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = Vector2(1.0f, 0.0f);

	vertex[2].Position = Vector3(0.0f, 200.0f, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = Vector2(0.0f, 1.0f);

	vertex[3].Position = Vector3(200.0f, 200.0f, 0.0f);
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

}

void InitTimerCnt() {
	g_timer.timer = 0;
}

void UpdateTimer() {
	//�t�F�[�h���I�������^�C�}�[�X�^�[�g
	Fade* fade = GetFade();
	if (fade->BS_Fadecnt >= (fade->BS_FadeOutTime * 60)) {
		g_timer.timer++;
	}
}

void DrawTimerDecimal() {

	// ����������
	Renderer::SetBlendState(BS_ALPHABLEND);

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

	int count = g_timer.timer / 0.6f;

	for (int i = 0; i < 2; i++)
	{
		int number = count % 10;
		count /= 10;

		//�e�N�X�`�����W�Z�o
		float tx = number % 5 * (1.0f / 5);
		float ty = number / 5 * (1.0f / 5);

		//���_�f�[�^��������
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		float x = 320.0f - i * 40.0f;
		float y = 40.0f;
		float height = 100.0f;
		float width = 50.0f;

		vertex[0].Position = Vector3(x, y, 0.0f);
		vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = Vector2(tx, ty);

		vertex[1].Position = Vector3(x + width, y, 0.0f);
		vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = Vector2(tx + 0.2f, ty);

		vertex[2].Position = Vector3(x, y + height, 0.0f);
		vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = Vector2(tx, ty + 0.2f);

		vertex[3].Position = Vector3(x + width, y + height, 0.0f);
		vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = Vector2(tx + 0.2f, ty + 0.2f);

		Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

		// �|���S���`��
		Renderer::GetDeviceContext()->Draw(4, 0);
	}
}

void DrawTimerSec() {

	int number;

	// ����������
	Renderer::SetBlendState(BS_ALPHABLEND);

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

	int count = g_timer.timer / 60;

	for (int i = 0; i < 2; i++)
	{
		if (i == 0) {
			number = count % 10;
		}
		else{
			number = count % 6;
		}
		count /= 10;

		//�e�N�X�`�����W�Z�o
		float tx = number % 5 * (1.0f / 5);
		float ty = number / 5 * (1.0f / 5);

		//���_�f�[�^��������
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		float x = 220.0f - i * 40.0f;
		float y = 40.0f;
		float height = 100.0f;
		float width = 50.0f;

		vertex[0].Position = Vector3(x, y, 0.0f);
		vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = Vector2(tx, ty);

		vertex[1].Position = Vector3(x + width, y, 0.0f);
		vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = Vector2(tx + 0.2f, ty);

		vertex[2].Position = Vector3(x, y + height, 0.0f);
		vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = Vector2(tx, ty + 0.2f);

		vertex[3].Position = Vector3(x + width, y + height, 0.0f);
		vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = Vector2(tx + 0.2f, ty + 0.2f);

		Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

		// �|���S���`��
		Renderer::GetDeviceContext()->Draw(4, 0);
	}
}

void DrawTimerMin() {

	// ����������
	Renderer::SetBlendState(BS_ALPHABLEND);

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

	int count = g_timer.timer / 3600;

	for (int i = 0; i < 1; i++)
	{
		int number = count % 10;
		count /= 10;

		//�e�N�X�`�����W�Z�o
		float tx = number % 5 * (1.0f / 5);
		float ty = number / 5 * (1.0f / 5);

		//���_�f�[�^��������
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		float x = 50.0f;
		float y = 50.0f;
		float height = 100.0f;
		float width = 100.0f;

		vertex[0].Position = Vector3(x, y, 0.0f);
		vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = Vector2(tx, ty);

		vertex[1].Position = Vector3(x + width, y, 0.0f);
		vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = Vector2(tx + 0.2f, ty);

		vertex[2].Position = Vector3(x, y + height, 0.0f);
		vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = Vector2(tx, ty + 0.2f);

		vertex[3].Position = Vector3(x + width, y + height, 0.0f);
		vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = Vector2(tx + 0.2f, ty + 0.2f);

		Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

		// �|���S���`��
		Renderer::GetDeviceContext()->Draw(4, 0);
	}
}

void DrawClearTimeSec() {

	int number;

	// ����������
	Renderer::SetBlendState(BS_ALPHABLEND);

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

	int count = g_timer.timer / 60;

	for (int i = 0; i < 2; i++)
	{
		if (i == 0) {
			number = count % 10;
		}
		else {
			number = count % 6;
		}
		count /= 10;

		//�e�N�X�`�����W�Z�o
		float tx = number % 5 * (1.0f / 5);
		float ty = number / 5 * (1.0f / 5);

		//���_�f�[�^��������
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		float x = (SCREEN_WIDTH / 2) + 100 - (i * 100.0f);
		float y = SCREEN_HEIGHT / 2 + 100.0f;
		float height = 200.0f;
		float width = 200.0f;

		vertex[0].Position = Vector3(x, y, 0.0f);
		vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = Vector2(tx, ty);

		vertex[1].Position = Vector3(x + width, y, 0.0f);
		vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = Vector2(tx + 0.2f, ty);

		vertex[2].Position = Vector3(x, y + height, 0.0f);
		vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = Vector2(tx, ty + 0.2f);

		vertex[3].Position = Vector3(x + width, y + height, 0.0f);
		vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = Vector2(tx + 0.2f, ty + 0.2f);

		Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

		// �|���S���`��
		Renderer::GetDeviceContext()->Draw(4, 0);
	}
}

void DrawClearTimeMin() {

	// ����������
	Renderer::SetBlendState(BS_ALPHABLEND);

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

	int count = g_timer.timer / 3600;

	for (int i = 0; i < 1; i++)
	{
		int number = count % 10;
		count /= 10;

		//�e�N�X�`�����W�Z�o
		float tx = number % 5 * (1.0f / 5);
		float ty = number / 5 * (1.0f / 5);

		//���_�f�[�^��������
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		float x = SCREEN_WIDTH / 2 - 200.0f;
		float y = SCREEN_HEIGHT / 2 + 100.0f;
		float height = 200.0f;
		float width = 200.0f;

		vertex[0].Position = Vector3(x, y, 0.0f);
		vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = Vector2(tx, ty);

		vertex[1].Position = Vector3(x + width, y, 0.0f);
		vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = Vector2(tx + 0.2f, ty);

		vertex[2].Position = Vector3(x, y + height, 0.0f);
		vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = Vector2(tx, ty + 0.2f);

		vertex[3].Position = Vector3(x + width, y + height, 0.0f);
		vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = Vector2(tx + 0.2f, ty + 0.2f);

		Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

		// �|���S���`��
		Renderer::GetDeviceContext()->Draw(4, 0);
	}
}

void ExitTimer() {
	g_VertexBuffer->Release();
	g_material.Uninit();
	g_Texture->Release();
}