#include	<d3d11.h>
#include	<SimpleMath.h>
#include	<WICTextureLoader.h>
#include	"CDirectInput.h"
#include	"CShader.h"
#include	"dx11helper.h"
#include	"camera.h"
#include	"CMaterial.h"
#include	"Player.h"
#include	"sceneChange.h"
#include	"SceneFade.h"

using namespace DirectX::SimpleMath;

// 使用するシェーダー
static CShader g_shader;

// RTS情報
static DirectX::SimpleMath::Vector3	g_Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
static DirectX::SimpleMath::Vector3	g_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
static DirectX::SimpleMath::Vector3	g_Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

// 頂点バッファ
static ID3D11Buffer* g_VertexBuffer{};

// テクスチャリソース
static ID3D11ShaderResourceView* g_Texture{};

// マテリアル
static CMaterial	g_material{};

Fade f_fade;

Fade* GetFade() {
	return &f_fade;
}

void InitSceneFade() {

	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/unlitTextureVS.hlsl",				// 頂点シェーダ
		"shader/unlitTexturePS.hlsl");				// ピクセルシェーダ

	// テクスチャ読み込み
	DirectX::CreateWICTextureFromFile(
		Renderer::GetDevice(),
		L"assets/texture/Scene_Fade.png",
		nullptr,
		&g_Texture);

	assert(g_Texture);

	// 頂点バッファ生成
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

}

void InitFadeVal() {
	f_fade.BossFade = 1.0f;
	f_fade.BossFadecnt = 0;

	f_fade.BossEncImgR = 0.0f;
	f_fade.BossEncImgL = 0.0f;

	f_fade.BattleSceneFade = 1.0f;
	f_fade.BS_Fadecnt = 0;

	f_fade.BossDirectionFade = 0.0f;
}

void DrawBattleSceneFade() {

	// 半透明合成
	Renderer::SetBlendState(BS_ALPHABLEND);

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

	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;
	if (ReturnSceneNum() == 3) {
		f_fade.BattleSceneFade = 0.0f;
	}

	//シーン遷移したら
	if (f_fade.BattleSceneFade >= 0.0f&& ReturnSceneNum() == 1) {
		//フェードイン開始時間まで回す		更新し続けないように条件
		if (f_fade.BS_Fadecnt <= (f_fade.BS_FadeOutTime * 60.0f)) {
			f_fade.BS_Fadecnt++;
		}
	}

	//一定時間経過でフェードイン開始
	if (f_fade.BS_Fadecnt >= (f_fade.BS_FadeOutTime * 60.0f)) {
		if (f_fade.BattleSceneFade >= 0.0f) {
			f_fade.BattleSceneFade -= 1.0f / (f_fade.BS_FadeOutTime * 60.0f);		//６０フレームで回すので開始時間を６０で掛けている
		}
	}

	float x = 0;
	float y = 0;

	vertex[0].Position = Vector3(x, y, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, f_fade.BattleSceneFade);
	vertex[0].TexCoord = Vector2(0.0f, 2.0f / 3);

	vertex[1].Position = Vector3(SCREEN_WIDTH, y, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, f_fade.BattleSceneFade);
	vertex[1].TexCoord = Vector2(1.0f / 2, 2.0f / 3);

	vertex[2].Position = Vector3(x, SCREEN_HEIGHT, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, f_fade.BattleSceneFade);
	vertex[2].TexCoord = Vector2(0.0f, 1.0f);

	vertex[3].Position = Vector3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, f_fade.BattleSceneFade);
	vertex[3].TexCoord = Vector2(1.0f / 2, 1.0f);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// ポリゴン描画
	Renderer::GetDeviceContext()->Draw(4, 0);
}

void UpdateBossFadeVal() {

	if (ReturnSceneNum() == 4) {
		f_fade.BossFadecnt++;
		DrawBossFade();
		DrawBossDirection1();
		DrawBossDirection2();
		DrawBossDirection3();
	}
}

//以下ボスの演出用のコード------------------------------------------
void DrawBossFade() {

	// 半透明合成
	Renderer::SetBlendState(BS_ALPHABLEND);

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

	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	if (f_fade.BossFadecnt >= f_fade.BossDir1) {
		if (f_fade.BossFade >= 0.0f) {
			f_fade.BossFade -= 1.0f / 60;		//約1秒でフェードアウト
		}
	}

	float x = 0;
	float y = 0;

	vertex[0].Position = Vector3(x, y, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, f_fade.BossFade);
	vertex[0].TexCoord = Vector2(0.0f, 1.0f / 3);

	vertex[1].Position = Vector3(SCREEN_WIDTH, y, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, f_fade.BossFade);
	vertex[1].TexCoord = Vector2(1.0f / 2, 1.0f / 3);

	vertex[2].Position = Vector3(x, SCREEN_HEIGHT, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, f_fade.BossFade);
	vertex[2].TexCoord = Vector2(0.0f, 2.0f / 3);

	vertex[3].Position = Vector3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, f_fade.BossFade);
	vertex[3].TexCoord = Vector2(1.0f / 2, 2.0f / 3);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// ポリゴン描画
	Renderer::GetDeviceContext()->Draw(4, 0);
}

void DrawBossDirection1() {

	// 半透明合成
	Renderer::SetBlendState(BS_ALPHABLEND);

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

	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	if (f_fade.BossFadecnt >= f_fade.BossDir2) {
		if (f_fade.BossEncImgR <= SCREEN_WIDTH * 2) {
			f_fade.BossEncImgR += SCREEN_WIDTH / 60;
		}
	}

	vertex[0].Position = Vector3(-SCREEN_WIDTH + f_fade.BossEncImgR, 0, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = Vector2(0.0f, 0.0f);

	vertex[1].Position = Vector3(f_fade.BossEncImgR, 0, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = Vector2(1.0f / 2, 0.0f);

	vertex[2].Position = Vector3(-SCREEN_WIDTH+ f_fade.BossEncImgR, SCREEN_HEIGHT, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = Vector2(0.0f, 1.0f / 3);

	vertex[3].Position = Vector3(f_fade.BossEncImgR, SCREEN_HEIGHT, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = Vector2(1.0f / 2, 1.0f / 3);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// ポリゴン描画
	Renderer::GetDeviceContext()->Draw(4, 0);
}

void DrawBossDirection2() {

	// 半透明合成
	Renderer::SetBlendState(BS_ALPHABLEND);

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

	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	if (f_fade.BossFadecnt >= f_fade.BossDir2) {
		if (f_fade.BossEncImgL >= -SCREEN_WIDTH * 2) {
			f_fade.BossEncImgL -= SCREEN_WIDTH / 60;
		}
	}

	vertex[0].Position = Vector3(SCREEN_WIDTH + f_fade.BossEncImgL, 0, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = Vector2(1.0f/2, 0.0f);

	vertex[1].Position = Vector3(SCREEN_WIDTH*2 + f_fade.BossEncImgL, 0, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = Vector2(1.0f, 0.0f);

	vertex[2].Position = Vector3(SCREEN_WIDTH + f_fade.BossEncImgL, SCREEN_HEIGHT, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = Vector2(1.0f / 2, 1.0f / 3);

	vertex[3].Position = Vector3(SCREEN_WIDTH*2 + f_fade.BossEncImgL, SCREEN_HEIGHT, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = Vector2(1.0f, 1.0f / 3);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// ポリゴン描画
	Renderer::GetDeviceContext()->Draw(4, 0);
}

void DrawBossDirection3() {

	// 半透明合成
	Renderer::SetBlendState(BS_ALPHABLEND);

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

	//頂点データ書き換え
	D3D11_MAPPED_SUBRESOURCE msr;
	Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	if (f_fade.BossFadecnt >= f_fade.BossDir3 && f_fade.BossFadecnt <= f_fade.BossDirEnd) {
		if (f_fade.BossDirectionFade <= 1.0f) {
			f_fade.BossDirectionFade += 1.0f / 30;		//０．５秒でフェードイン
		}
	}

	if (f_fade.BossFadecnt >= f_fade.BossDirEnd) {
		if (f_fade.BossDirectionFade >= 0.0f) {
			f_fade.BossDirectionFade -= 1.0f / 60;	//1秒でフェードアウト
		}
	}

	float x = 0;
	float y = 0;

	vertex[0].Position = Vector3(x, y, 0.0f);
	vertex[0].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = Vector4(1.0f, 1.0f, 1.0f, f_fade.BossDirectionFade);
	vertex[0].TexCoord = Vector2(1.0f / 2, 1.0f / 3);

	vertex[1].Position = Vector3(SCREEN_WIDTH, y, 0.0f);
	vertex[1].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = Vector4(1.0f, 1.0f, 1.0f, f_fade.BossDirectionFade);
	vertex[1].TexCoord = Vector2(1.0f, 1.0f / 3);

	vertex[2].Position = Vector3(x, SCREEN_HEIGHT, 0.0f);
	vertex[2].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = Vector4(1.0f, 1.0f, 1.0f, f_fade.BossDirectionFade);
	vertex[2].TexCoord = Vector2(1.0f / 2, 2.0f / 3);

	vertex[3].Position = Vector3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	vertex[3].Normal = Vector3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = Vector4(1.0f, 1.0f, 1.0f, f_fade.BossDirectionFade);
	vertex[3].TexCoord = Vector2(1.0f, 2.0f / 3);

	Renderer::GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	// ポリゴン描画
	Renderer::GetDeviceContext()->Draw(4, 0);
}
//------------------------------------------------------------------
void ExitSceneFade() {
	g_VertexBuffer->Release();
	g_material.Uninit();
	g_Texture->Release();
}

int BossFadeCount() {
	return f_fade.BossFadecnt;
}