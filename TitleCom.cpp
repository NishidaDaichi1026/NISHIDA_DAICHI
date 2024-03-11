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

static float ResizeCom;			//コマンドの画像の大きさ変更
static int selectCom = 1;		//選択されているコマンド
static float transparent = 0;	//透過率変更

//コマンドの変数を返す
int GameSelectNum() {
	return selectCom;
}

void InitTitleCom() {

	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/unlitTextureVS.hlsl",				// 頂点シェーダ
		"shader/unlitTexturePS.hlsl");				// ピクセルシェーダ

	// テクスチャ読み込み
	DirectX::CreateWICTextureFromFile(
		Renderer::GetDevice(),
		L"assets/texture/TITLE_COM.png",
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

	//タイトルのBGM
	InitSound();
	PlaySound(BGM1);

}

//コマンド選択とシーン遷移
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

	// リターンキー入力で画面遷移
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_RETURN)) {
		StopSound(BGM1);
		switch (selectCom){
		//戦闘シーン
		case 1:
			//プレイヤーと敵のステータスを初期化
			InitPlayerState();
			InitEnemiesState();
			InitBoss_ST();
			//演出＆タイマーに使用する変数の初期化
			InitFadeVal();
			InitTimerCnt();
			
			SetScene(1);
			break;
		//操作・ゲーム性などの説明をするシーンに遷移
		case 2:
			//プレイヤーと敵のステータスを初期化
			InitPlayerState();
			InitTimerCnt();
			SetScene(3);
			break;
		//ゲーム終了
		case 3:
			break;
		//デバック用（selectCom変数が意図しない数字になったときに何もならないように）
		default:
			break;
		}
	}
}

void DrawTitleCom() {

	// 半透明合成(PNGなどの透過画像)
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

	for (int i = 1; i < 4; i++){

		//テクスチャ座標算出
		float ty = (i - 1) * (1.0f / 3);

		//頂点データ書き換え
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(g_VertexBuffer, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;
		//画像の位置
		float x = SCREEN_WIDTH - 650.0f - i * 100.0f;
		float y = SCREEN_HEIGHT - 500.0f + i * 120.0f;
		//画像の大きさ
		float height = 150.0f;
		float width = 500.0f;

		//選択されていると透過しない&サイズを大きくする
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

		// ポリゴン描画
		Renderer::GetDeviceContext()->Draw(4, 0);
	}
}

void ExitTitleCom() {
	g_VertexBuffer->Release();
	g_material.Uninit();
	g_Texture->Release();
}