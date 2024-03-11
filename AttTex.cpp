#include	<d3d11.h>
#include	<SimpleMath.h>
#include	<WICTextureLoader.h>
#include	"CDirectInput.h"
#include	"CShader.h"
#include	"dx11helper.h"
#include	"camera.h"
#include	"CMaterial.h"
#include	"game.h"
#include	"Player.h"
#include	"State.h"

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

static float AT_transparent = 1.0f;		//�U���R�}���h�̓��ߗ�
static float SP_transparent = 1.0f;		//�X�L���R�}���h�̓��ߗ�

extern ATTRIBUTE attr[];

void InitAttTex() {

	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/unlitTextureVS.hlsl",				// ���_�V�F�[�_
		"shader/unlitTexturePS.hlsl");				// �s�N�Z���V�F�[�_

	// �e�N�X�`���ǂݍ���
	DirectX::CreateWICTextureFromFile(
		Renderer::GetDevice(),
		L"assets/texture/ATTR_TEX.png",
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

//�v���C���[�̑����A�C�R��
void DrawAttTex() {

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

	for (int i = 0; i < 5; i++) {

		//�e�N�X�`�����W�Z�o
		float tx = i * (1.0f / 5);

		//���_�f�[�^��������
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;
		//�摜�̈ʒu
		float x = i * 200.0f;
		float y = SCREEN_HEIGHT - 200.0f;
		//�摜�̑傫��
		float height = 200.0f;
		float width = 200.0f;

		vertex[0].Position = Vector3(x, y, 0.0f);
		vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = Vector2(tx, 0.0f);

		vertex[1].Position = Vector3(x + width, y, 0.0f);
		vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = Vector2(tx + 1.0f / 5, 0.0f);

		vertex[2].Position = Vector3(x, y + height, 0.0f);
		vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = Vector2(tx, 1.0f / 3);

		vertex[3].Position = Vector3(x + width, y + height, 0.0f);
		vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = Vector2(tx + 1.0f / 5, 1.0f / 3);

		Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

		// �|���S���`��
		Renderer::GetDeviceContext()->Draw(4, 0);
	}
}
//�U���A�C�R��
void DrawAttackCom() {
	//�摜�̑傫���ύX
	float Resize;

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

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_E) ||
		CDirectInput::GetInstance().GetMouseLButtonCheck()) {
		AT_transparent = 0.6f;
		Resize = 20.0f;
	}
	else{
		AT_transparent = 1.0f;
		Resize = 0.0f;
	}

	//�e�N�X�`�����W�Z�o
	float tx = 0.0f;

	//���_�f�[�^��������
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;
	//�摜�̈ʒu
	float x = SCREEN_WIDTH - 375;
	float y = SCREEN_HEIGHT - 325;
	//�摜�̑傫��
	float height = 170.0f;
	float width = 170.0f;

	vertex[0].Position = Vector3(x - Resize, y - Resize, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, AT_transparent);
	vertex[0].TexCoord = Vector2(tx, 2.0f / 3);

	vertex[1].Position = Vector3(x + width + Resize, y - Resize, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, AT_transparent);
	vertex[1].TexCoord = Vector2(tx + 1.0f / 5, 2.0f / 3);

	vertex[2].Position = Vector3(x - Resize, y + height + Resize, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, AT_transparent);
	vertex[2].TexCoord = Vector2(tx, 1.0f);

	vertex[3].Position = Vector3(x + width + Resize, y + height + Resize, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, AT_transparent);
	vertex[3].TexCoord = Vector2(tx + 1.0f / 5, 1.0f);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// �|���S���`��
	Renderer::GetDeviceContext()->Draw(4, 0);
}
//�K�E�Z�̃A�C�R��
void DrawSpecialCom() {
	//�摜�̑傫���ύX
	float Resize;

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
	
	//�v���C���[�̍U�������擾
	PlayerAbility* P_abi = GetPlayerAbi();
	//�e�X�L�������܂��Ă��邩
	Player* g_player = GetPlayer();
	//�K�E�Z�̃A�C�R���̓��ߗ�
	switch (P_abi->A_ID){
	case 0:
		if (g_player->charge_N < 3) {
			SP_transparent = 0.6f;
		}else{
			SP_transparent = 1.0f;
		}
		break;
	case 1:
		if (g_player->charge_F < 3) {
			SP_transparent = 0.6f;
		}
		else {
			SP_transparent = 1.0f;
		}
		break;
	case 2:
		if (g_player->charge_W < 3) {
			SP_transparent = 0.6f;
		}
		else {
			SP_transparent = 1.0f;
		}
		break;
	case 3:
		if (g_player->charge_I < 3) {
			SP_transparent = 0.6f;
		}
		else {
			SP_transparent = 1.0f;
		}
		break;
	case 4:
		if (g_player->charge_T < 3) {
			SP_transparent = 0.6f;
		}
		else {
			SP_transparent = 1.0f;
		}
		break;
	default:
		break;
	}
	
	if(CDirectInput::GetInstance().CheckKeyBuffer(DIK_Q)) {
		Resize = 20.0f;
	}
	else{
		Resize = 0.0f;
	}

	//�e�N�X�`�����W�Z�o
	float tx = 1.0f / 5;

	//���_�f�[�^��������
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;
	//�摜�̈ʒu
	float x = SCREEN_WIDTH - 250;
	float y = SCREEN_HEIGHT - 450;
	//�摜�̑傫��
	float height = 170.0f;
	float width = 170.0f;

	vertex[0].Position = Vector3(x- Resize, y- Resize, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, SP_transparent);
	vertex[0].TexCoord = Vector2(tx, 2.0f / 3);

	vertex[1].Position = Vector3(x + width+ Resize, y- Resize, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, SP_transparent);
	vertex[1].TexCoord = Vector2(tx + 1.0f / 5, 2.0f / 3);

	vertex[2].Position = Vector3(x- Resize, y + height+ Resize, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, SP_transparent);
	vertex[2].TexCoord = Vector2(tx, 1.0f);

	vertex[3].Position = Vector3(x + width+ Resize, y + height+ Resize, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, SP_transparent);
	vertex[3].TexCoord = Vector2(tx + 1.0f / 5, 1.0f);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// �|���S���`��
	Renderer::GetDeviceContext()->Draw(4, 0);
}
//���݂̍U���������킩��₷���悤�ɉ摜�\��
void DrawSelCom() {
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

	PlayerAbility* g_player = GetPlayerAbi();
	int NUM = g_player->A_ID;

	//���_�f�[�^��������
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float x = NUM * 200.0f;
	float y = SCREEN_HEIGHT - 200.0f;
	float height = 200.0f;
	float width = 200.0f;

	vertex[0].Position = Vector3(x, y, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = Vector2(2.0f / 5, 2.0f / 3);

	vertex[1].Position = Vector3(x + width, y, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = Vector2(3.0f/5, 2.0f / 3);

	vertex[2].Position = Vector3(x, y + height, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = Vector2(2.0f/5, 1.0f);

	vertex[3].Position = Vector3(x + width, y + height, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = Vector2(3.0f/5, 1.0f);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// �|���S���`��
	Renderer::GetDeviceContext()->Draw(4, 0);
}
//---------------------------------------------------------------------------------
//�e�X�L�����g�p�\���iGO�Ō��Ă�j
void DrawAttTex_N() {

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

	Player* g_player = GetPlayer();
	int N_NUM = g_player->charge_N;

	//�e�N�X�`�����W�Z�o
	float tx = N_NUM * (1.0f / 5);

	//���_�f�[�^��������
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float x = attr[0].ID * 200.0f;
	float y = SCREEN_HEIGHT - 200.0f;
	float height = 200.0f;
	float width = 200.0f;

	vertex[0].Position = Vector3(x, y, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = Vector2(tx, 1.0f / 3);

	vertex[1].Position = Vector3(x + width, y, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = Vector2(tx + 1.0f / 5, 1.0f / 3);

	vertex[2].Position = Vector3(x, y + height, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = Vector2(tx, 2.0f / 3);

	vertex[3].Position = Vector3(x + width, y + height, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = Vector2(tx + 1.0f / 5, 2.0f / 3);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// �|���S���`��
	Renderer::GetDeviceContext()->Draw(4, 0);
}

void DrawAttTex_F() {

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

	Player* g_player = GetPlayer();
	int F_NUM = g_player->charge_F;

	//�e�N�X�`�����W�Z�o
	float tx = F_NUM * (1.0f / 5);

	//���_�f�[�^��������
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float x = attr[1].ID * 200.0f;
	float y = SCREEN_HEIGHT - 200.0f;
	float height = 200.0f;
	float width = 200.0f;

	vertex[0].Position = Vector3(x, y, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = Vector2(tx, 1.0f / 3);

	vertex[1].Position = Vector3(x + width, y, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = Vector2(tx + 1.0f / 5, 1.0f / 3);

	vertex[2].Position = Vector3(x, y + height, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = Vector2(tx, 2.0f / 3);

	vertex[3].Position = Vector3(x + width, y + height, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = Vector2(tx + 1.0f / 5, 2.0f / 3);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// �|���S���`��
	Renderer::GetDeviceContext()->Draw(4, 0);
}

void DrawAttTex_W() {

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

	Player* g_player = GetPlayer();
	int W_NUM = g_player->charge_W;

	//�e�N�X�`�����W�Z�o
	float tx = W_NUM * (1.0f / 5);

	//���_�f�[�^��������
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float x = attr[2].ID * 200.0f;
	float y = SCREEN_HEIGHT - 200.0f;
	float height = 200.0f;
	float width = 200.0f;

	vertex[0].Position = Vector3(x, y, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = Vector2(tx, 1.0f / 3);

	vertex[1].Position = Vector3(x + width, y, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = Vector2(tx + 1.0f / 5, 1.0f / 3);

	vertex[2].Position = Vector3(x, y + height, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = Vector2(tx, 2.0f / 3);

	vertex[3].Position = Vector3(x + width, y + height, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = Vector2(tx + 1.0f / 5, 2.0f / 3);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// �|���S���`��
	Renderer::GetDeviceContext()->Draw(4, 0);
}

void DrawAttTex_I() {

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

	Player* g_player = GetPlayer();
	int I_NUM = g_player->charge_I;

	//�e�N�X�`�����W�Z�o
	float tx = I_NUM * (1.0f / 5);

	//���_�f�[�^��������
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float x = attr[3].ID * 200.0f;
	float y = SCREEN_HEIGHT - 200.0f;
	float height = 200.0f;
	float width = 200.0f;

	vertex[0].Position = Vector3(x, y, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = Vector2(tx, 1.0f / 3);

	vertex[1].Position = Vector3(x + width, y, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = Vector2(tx + 1.0f / 5, 1.0f / 3);

	vertex[2].Position = Vector3(x, y + height, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = Vector2(tx, 2.0f / 3);

	vertex[3].Position = Vector3(x + width, y + height, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = Vector2(tx + 1.0f / 5, 2.0f / 3);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// �|���S���`��
	Renderer::GetDeviceContext()->Draw(4, 0);
}

void DrawAttTex_T() {

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

	Player* g_player = GetPlayer();
	int T_NUM = g_player->charge_T;

	//�e�N�X�`�����W�Z�o
	float tx = T_NUM * (1.0f / 5);

	//���_�f�[�^��������
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float x = attr[4].ID * 200.0f;
	float y = SCREEN_HEIGHT - 200.0f;
	float height = 200.0f;
	float width = 200.0f;

	vertex[0].Position = Vector3(x, y, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = Vector2(tx, 1.0f / 3);

	vertex[1].Position = Vector3(x + width, y, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = Vector2(tx + 1.0f / 5, 1.0f / 3);

	vertex[2].Position = Vector3(x, y + height, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = Vector2(tx, 2.0f / 3);

	vertex[3].Position = Vector3(x + width, y + height, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = Vector2(tx + 1.0f / 5, 2.0f / 3);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// �|���S���`��
	Renderer::GetDeviceContext()->Draw(4, 0);
}
//---------------------------------------------------------------------------------
void ExitAttTex() {
	g_VertexBuffer->Release();
	g_material.Uninit();
	g_Texture->Release();
}