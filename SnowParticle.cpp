//=============================================================================
// パーティクル処理 [particle.cpp]
//=============================================================================
#include	<d3d11.h>
#include	<wrl/client.h>
#include	<DirectXMath.h>
#include	<random>
#include	"renderer.h"
#include	"CDirectInput.h"
#include	"dx11helper.h"
#include	"SnowParticle.h"
#include	"camera.h"
#include	"CMaterial.h"
#include	"CPlaneMesh.h"
#include	"CShader.h"
#include	"CMeshRenderer.h"
#include	"Player.h"

using namespace DirectX;

using namespace DirectX::SimpleMath;

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TEXTURE_PARTICLE	"data/texture/snow.png"	// 読み込むテクスチャファイル名
#define	PARTICLE_SIZE_X		(20.0f)							// ビルボードの幅
#define	PARTICLE_SIZE_Y		(20.0f)							// ビルボードの高さ
#define	VALUE_MOVE_PARTICLE	(2.0f)							// 移動速度

#define	MAX_PARTICLE		(512)							// エフェクト最大数

#define MATERIAL_DIFFUSE	XMFLOAT4(1,1,1,1)
#define MATERIAL_AMBIENT	XMFLOAT4(0,0,0,1)
#define MATERIAL_SPECULAR	XMFLOAT4(0,0,0,1)
#define MATERIAL_EMISSIVE	XMFLOAT4(0,0,0,1)
#define MATERIAL_POWER		(0.0f)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PARTICLE{
	XMFLOAT3 pos;			// 位置
	XMFLOAT3 rot;			// 回転
	XMFLOAT3 move;			// 移動量
	XMFLOAT4 col;			// 色
	float fSizeX;			// 幅
	float fSizeY;			// 高さ
	int nIdxShadow;			// 影ID
	int nLife;				// 寿命
	bool bUse;				// 使用しているかどうか
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetVertexSnowParticle(int nIdxParticle, float fSizeX, float fSizeY);
void SetColorSnowParticle(int nIdxParticle, XMFLOAT4 col);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CPlaneMesh		g_meshParticle;					// メッシュ情報
static CMaterial		g_material;						// マテリアル

static PARTICLE			g_aParticle[MAX_PARTICLE];		// パーティクルワーク
static XMFLOAT3			g_posBase;						// ビルボード発生位置

static float			g_fWidthBase = 5.0f;			// 基準の幅
static float			g_fHeightBase = 10.0f;			// 基準の高さ
static bool				g_bPause = false;				// ポーズON/OFF

// SRV（テクスチャ）
static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_srv;

// 使用するシェーダ
static CShader			g_shader;

// レンダラ
static CMeshRenderer	g_meshrenderer;

//=============================================================================
// 初期化処理
//=============================================================================
bool InitSnowParticle(void)
{
	ID3D11Device* pDevice = Renderer::GetDevice();
	ID3D11DeviceContext* pDeviceContext = Renderer::GetDeviceContext();

	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/unlitTextureVS.hlsl",				// 頂点シェーダ
		"shader/unlitTexturePS.hlsl");				// ピクセルシェーダ

	// メッシュ生成
	g_meshParticle.Init(
		1, 1,					// 分割数
		1,						// サイズ
		1,						// サイズ
		Color(1, 1, 1, 1),		// 頂点カラー
		Vector3(0, 0, -1),		// 法線ベクトル
		false);					// XZ平面

	// メッシュレンダラ生成（床用）
	g_meshrenderer.Init(g_meshParticle);

	ID3D11Device* dev;
	ID3D11DeviceContext* devcontext;

	dev = Renderer::GetDevice();
	devcontext = Renderer::GetDeviceContext();

	bool sts = CreateSRVfromFile("assets/texture/snow.png", dev, devcontext, &g_srv);
	if (!sts) {
		MessageBox(nullptr, "CreateSRVfromFile error(MeshWallInit)", "error", MB_OK);
	}

	// マテリアル初期化
	MATERIAL mtrl;
	mtrl.Ambient = Color(1, 1, 1, 1);
	mtrl.Diffuse = Color(1, 1, 1, 1);
	mtrl.Emission = Color(0, 0, 0, 0);
	mtrl.Specular = Color(0, 0, 0, 0);
	mtrl.Shininess = 0;
	mtrl.TextureEnable = TRUE;
	g_material.Init(mtrl);

	// パーティクル情報初期化
	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; ++nCntParticle) {
		g_aParticle[nCntParticle].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].move = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].fSizeX = PARTICLE_SIZE_X;
		g_aParticle[nCntParticle].fSizeY = PARTICLE_SIZE_Y;
		g_aParticle[nCntParticle].nIdxShadow = -1;
		g_aParticle[nCntParticle].nLife = 0;
		g_aParticle[nCntParticle].bUse = false;
	}

	return true;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSnowParticle(void)
{
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSnowParticle(void)
{
	XMFLOAT3 rotCamera;

	Vector3	 posModel;				// モデル座標

	//プレイヤーの座標を取得
	posModel = GetPositionPlayer();
	// カメラの回転を取得
	rotCamera = Camera::GetRotCamera();

	// Pキーでポーズ（トグルスイッチ切り替え）
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_P)) {
		g_bPause = !g_bPause;
	}

	// ポーズでなければ
	if (!g_bPause) {
		for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; ++nCntParticle) {
			if (g_aParticle[nCntParticle].bUse) {

				// 使用中
				g_aParticle[nCntParticle].pos.x += g_aParticle[nCntParticle].move.x;
				g_aParticle[nCntParticle].pos.z += g_aParticle[nCntParticle].move.z;

				g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y / 20;
				if (g_aParticle[nCntParticle].pos.y <= g_aParticle[nCntParticle].fSizeY / 2) {
				}

				g_aParticle[nCntParticle].move.x += (0.0f - g_aParticle[nCntParticle].move.x) * 0.015f;
				g_aParticle[nCntParticle].move.y -= 0.25f;
				g_aParticle[nCntParticle].move.z += (0.0f - g_aParticle[nCntParticle].move.z) * 0.015f;

				g_aParticle[nCntParticle].nLife--;
				if (g_aParticle[nCntParticle].nLife <= 0) {
					g_aParticle[nCntParticle].bUse = false;
					g_aParticle[nCntParticle].nIdxShadow = -1;
				} else {
					if (g_aParticle[nCntParticle].nLife <= 20) {
						// α値設定
						g_aParticle[nCntParticle].col.w -= 0.05f;
						if (g_aParticle[nCntParticle].col.w < 0.0f) {
							g_aParticle[nCntParticle].col.w = 0.0f;
						}
					}

					// 色の設定
					SetColorSnowParticle(nCntParticle, g_aParticle[nCntParticle].col);
				}
			}
		}

		std::mt19937 rnd;
		std::uniform_real_distribution<float> dist(0.0f,XM_2PI);

		{
			XMFLOAT3 pos;
			XMFLOAT3 move;
			float fAngle, fLength;
			int nLife;
			float fSize;

			pos = g_posBase;

			fAngle = dist(rnd) - XM_PI;						// ラジアン

			fLength = rand() % (int)(g_fWidthBase * 200) / 100.0f - g_fWidthBase;

			move.x = sinf(fAngle) * fLength;

			move.y = rand() % 200 / 100.0f + g_fHeightBase;

			move.z = rand() % (int)(g_fWidthBase * 200) / 100.0f - g_fWidthBase;

			nLife = rand() % 100 + 150;

			fSize = (float)(rand() % 30 + 10);

			pos.y = (fSize / 2) + 100.0f;

			pos.x = posModel.x;
			pos.z = posModel.z;

			// ビルボードの設定
			SetSnowParticle(pos, move, XMFLOAT4(0.3f, 0.4f, 0.6f, 1.0f), fSize, fSize, nLife);
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSnowParticle(void)
{
	ID3D11DeviceContext* pDeviceContext = Renderer::GetDeviceContext();

	Renderer::SetBlendState(2);

	// ビューマトリックスを取得
	DirectX::SimpleMath::Matrix mtxView = Camera::GetViewMtx();

	// パーティクル用シェーダセット
	g_shader.SetGPU();

	// テクスチャをセット
	ID3D11DeviceContext* devcontext;
	devcontext = Renderer::GetDeviceContext();
	devcontext->PSSetShaderResources(0, 1, g_srv.GetAddressOf());

	// 全パーティクルをビルボードとして描画する
	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; ++nCntParticle) {
		if (g_aParticle[nCntParticle].bUse) {
	
			// ワールドマトリックスの初期化
			Matrix mtxWorld = Matrix::Identity;

			// ビルボード用行列作成
			mtxWorld._11 = mtxView._11;
			mtxWorld._12 = mtxView._21;
			mtxWorld._13 = mtxView._31;
			mtxWorld._21 = mtxView._12;
			mtxWorld._22 = mtxView._22;
			mtxWorld._23 = mtxView._32;
			mtxWorld._31 = mtxView._13;
			mtxWorld._32 = mtxView._23;
			mtxWorld._33 = mtxView._33;

			// スケール行列を作成
			Matrix mtxScale = Matrix::CreateScale(g_aParticle[nCntParticle].fSizeX, g_aParticle[nCntParticle].fSizeY, 1.0f);

			// 平行移動行列を作成
			Matrix mtxTranslate = Matrix::CreateTranslation(g_aParticle[nCntParticle].pos.x, g_aParticle[nCntParticle].pos.y, g_aParticle[nCntParticle].pos.z);

			// SRT生成
			Matrix mtx =  mtxScale * mtxWorld * mtxTranslate;

			// GPUに行列をセットする
			Renderer::SetWorldMatrix(&mtx);

			// カラーの設定
			g_material.SetDiffuse(g_aParticle[nCntParticle].col);

			// マテリアル
			MATERIAL mtrl;
			mtrl.Ambient = Color(1, 1, 1, 1);
			mtrl.Diffuse = g_aParticle[nCntParticle].col;
			mtrl.Emission = Color(0, 0, 0, 0);
			mtrl.Specular = Color(0, 0, 0, 0);
			mtrl.Shininess = 0;
			mtrl.TextureEnable = TRUE;

			g_material.SetMaterial(mtrl);

			// 床描画
			g_meshrenderer.Draw();

		}
	}

	// 半透明合成無
	Renderer::SetBlendState(0);
}

//=============================================================================
// 頂点座標の設定
//=============================================================================
void SetVertexSnowParticle(int nIdxParticle, float fSizeX, float fSizeY)
{
	if (nIdxParticle >= 0 && nIdxParticle < MAX_PARTICLE) {
		g_aParticle[nIdxParticle].fSizeX = fSizeX;
		g_aParticle[nIdxParticle].fSizeY = fSizeY;
	}
}

//=============================================================================
// 頂点カラーの設定
//=============================================================================
void SetColorSnowParticle(int nIdxParticle, XMFLOAT4 col)
{
	if (nIdxParticle >= 0 && nIdxParticle < MAX_PARTICLE) {
		g_aParticle[nIdxParticle].col = col;
	}
}

//=============================================================================
// エフェクトの設定
//=============================================================================
int SetSnowParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; ++nCntParticle) {
		if (!g_aParticle[nCntParticle].bUse) {
			g_aParticle[nCntParticle].pos = pos;
			g_aParticle[nCntParticle].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_aParticle[nCntParticle].move = move;
			g_aParticle[nCntParticle].nLife = nLife;
			g_aParticle[nCntParticle].bUse = true;

			// 頂点座標の設定
			SetVertexSnowParticle(nCntParticle, fSizeX, fSizeY);

			// 頂点カラーの設定
			SetColorSnowParticle(nCntParticle, col);

			nIdxParticle = nCntParticle;

			break;
		}
	}

	return nIdxParticle;
}

