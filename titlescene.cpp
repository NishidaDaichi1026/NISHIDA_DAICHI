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

// 使用するシェーダー
static CShader g_shader;

// RTS情報---------------------------------------------------------------------------------------------

static DirectX::SimpleMath::Vector3	g_Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
static DirectX::SimpleMath::Vector3	g_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
static DirectX::SimpleMath::Vector3	g_Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

//-----------------------------------------------------------------------------------------------------

// 頂点バッファ
ID3D11Buffer* g_VertexBuffer = nullptr;
// テクスチャリソース
ID3D11ShaderResourceView* g_Texture = nullptr;
// マテリアル
static CMaterial	g_material{};

bool InitTitleScene() {

	float width = SCREEN_WIDTH;
	float height = SCREEN_HEIGHT;

	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/unlitTextureVS.hlsl",				// 頂点シェーダ
		"shader/unlitTexturePS.hlsl");				// ピクセルシェーダ

	// テクスチャ読み込み
	DirectX::CreateWICTextureFromFile(
		Renderer::GetDevice(),
		L"assets/texture/白116-1536x864.jpg",
		nullptr,
		&g_Texture);

	assert(g_Texture);

	// 頂点バッファ生成
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

	// 頂点バッファ生成
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

	// マテリアル初期化
	MATERIAL mtrl;
	mtrl.Ambient = Color(1, 1, 1, 1);
	mtrl.Diffuse = Color(1, 1, 1, 1);
	mtrl.Emission = Color(0, 0, 0, 0);
	mtrl.Specular = Color(0, 0, 0, 0);
	mtrl.Shininess = 0;
	mtrl.TextureEnable = TRUE;

	g_material.Init(mtrl);

	//タイトル画像
	InitTitleCom();
	InitTitleImg();

	return true;
}

void UpdateTitleScene() {
	//タイトル画像
	UpdateTitleCom();
}

void DrawTitleScene() {

	// 2D描画用のカメラ処理実行
	Camera::Draw2D();

	// シェーダーをGPUにセット
	g_shader.SetGPU();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マテリアル設定
	g_material.SetGPU();

	// テクスチャ設定
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture);

	// プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// ポリゴン描画
	Renderer::GetDeviceContext()->Draw(4, 0);
	
	//タイトル画像
	DrawTitleCom();
	DrawTitleImg();
	DrawTitleLogo();
	DrawInstOp();
}

void ExitTitleScene() {
	g_VertexBuffer->Release();
	g_material.Uninit();
	g_Texture->Release();
	//タイトル画像
	ExitTitleCom();
	ExitTitleImg();
}