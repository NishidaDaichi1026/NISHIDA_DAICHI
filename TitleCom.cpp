#include	<d3d11.h>
#include	<SimpleMath.h>
#include	<WICTextureLoader.h>
#include	"CDirectInput.h"
#include	"CShader.h"
#include	"dx11helper.h"
#include	"camera.h"
#include	"CMaterial.h"
#include	"game.h"
#include	"sceneChange.h"
#include	"SummarizeEnemy.h"
#include	"Player.h"
#include	"BossEnemy.h"
#include	"SceneFade.h"
#include	"Timer.h"
#include	"XAudio2.h"


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

static float ResizeCom;			//�R�}���h�̉摜�̑傫���ύX
static int selectCom = 1;		//�I������Ă���R�}���h
static float transparent = 0;	//���ߗ��ύX

//�R�}���h�̕ϐ���Ԃ�
int GameSelectNum() {
	return selectCom;
}

void InitTitleCom() {

	// �g�p����V�F�[�_�[�𐶐�
	g_shader.SetShader(
		"shader/unlitTextureVS.hlsl",				// ���_�V�F�[�_
		"shader/unlitTexturePS.hlsl");				// �s�N�Z���V�F�[�_

	// �e�N�X�`���ǂݍ���
	DirectX::CreateWICTextureFromFile(
		Renderer::GetDevice(),
		L"assets/texture/TITLE_COM.png",
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

	//�^�C�g����BGM
	InitSound();
	PlaySound(BGM1);

}

//�R�}���h�I���ƃV�[���J��
void UpdateTitleCom() {
	if (selectCom > 1) {
		if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_UP)) {
			selectCom -= 1;
			PlaySound(SE2);
		}
	}
	if (selectCom < 3) {
		if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_DOWN)) {
			selectCom += 1;
			PlaySound(SE2);
		}
	}

	// ���^�[���L�[���͂ŉ�ʑJ��
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_RETURN)) {
		StopSound(BGM1);
		switch (selectCom){
		//�퓬�V�[��
		case 1:
			//�v���C���[�ƓG�̃X�e�[�^�X��������
			InitPlayerState();
			InitEnemiesState();
			InitBoss_ST();
			//���o���^�C�}�[�Ɏg�p����ϐ��̏�����
			InitFadeVal();
			InitTimerCnt();
			
			SetScene(1);
			break;
		//����E�Q�[�����Ȃǂ̐���������V�[���ɑJ��
		case 2:
			//�v���C���[�ƓG�̃X�e�[�^�X��������
			InitPlayerState();
			InitTimerCnt();
			SetScene(3);
			break;
		//�Q�[���I��
		case 3:
			break;
		//�f�o�b�N�p�iselectCom�ϐ����Ӑ}���Ȃ������ɂȂ����Ƃ��ɉ����Ȃ�Ȃ��悤�Ɂj
		default:
			break;
		}
	}
}

void DrawTitleCom() {

	// ����������(PNG�Ȃǂ̓��߉摜)
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

	for (int i = 1; i < 4; i++){

		//�e�N�X�`�����W�Z�o
		float ty = (i - 1) * (1.0f / 3);

		//���_�f�[�^��������
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;
		//�摜�̈ʒu
		float x = SCREEN_WIDTH - 650.0f - i * 100.0f;
		float y = SCREEN_HEIGHT - 500.0f + i * 120.0f;
		//�摜�̑傫��
		float height = 150.0f;
		float width = 500.0f;

		//�I������Ă���Ɠ��߂��Ȃ�&�T�C�Y��傫������
		if (selectCom == i) {
			transparent = 1.0f;
			ResizeCom = 15.0f;
		}
		else{
			transparent = 0.8f;
			ResizeCom = 0.0f;
		}

		vertex[0].Position = Vector3(x - ResizeCom * 2, y - ResizeCom, 0.0f);
		vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, transparent);
		vertex[0].TexCoord = Vector2(0.0f, ty);

		vertex[1].Position = Vector3(x + width + ResizeCom * 2, y - ResizeCom, 0.0f);
		vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, transparent);
		vertex[1].TexCoord = Vector2(1.0f, ty);

		vertex[2].Position = Vector3(x - ResizeCom * 2, y + height + ResizeCom, 0.0f);
		vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, transparent);
		vertex[2].TexCoord = Vector2(0.0f, ty + 1.0f / 3);

		vertex[3].Position = Vector3(x + width + ResizeCom * 2, y + height + ResizeCom, 0.0f);
		vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, transparent);
		vertex[3].TexCoord = Vector2(1.0f, ty + 1.0f / 3);

		Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

		// �|���S���`��
		Renderer::GetDeviceContext()->Draw(4, 0);
	}
}

void ExitTitleCom() {
	g_VertexBuffer->Release();
	g_material.Uninit();
	g_Texture->Release();
}