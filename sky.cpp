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
// マクロ定義
//*****************************************************************************
constexpr	int MAX_MESH_SKY = 5;									// 壁の総数

static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_srv;
static Microsoft::WRL::ComPtr<ID3D11Resource> g_texres;

static Vector3 g_scale = { 1,1,1 };

static float skySize = 1024.0f;		//サイズ

static Vector3 g_position[MAX_MESH_SKY] = {
	{0,0,skySize / 2},
	{-skySize / 2,0,0},
	{skySize / 2,0,0},
	{0,0,-skySize / 2},
	{0,skySize / 2,0}
};

static Vector3 g_rotation[MAX_MESH_SKY] = {
	{0,0,180},
	{180,90.0f,0},
	{180,-90.0f,0},
	{0,180.0f,180},
	{-90,0,0}
};

// 壁用
static CShader		g_shadersky;
//  壁メッシュ（ジオメトリデータ）
static CPlaneMesh g_skymesh;
// 壁メッシュ（レンダリングデータ）
static CMeshRenderer g_meshrenderer;
// 壁用マテリアル
static CMaterial	g_material;

// 初期化
void InitSky() {

	ID3D11Device* dev;
	ID3D11DeviceContext* devcontext;

	dev = Renderer::GetDevice();
	devcontext = Renderer::GetDeviceContext();

	bool sts = CreateSRVfromFile("assets/texture/FS002_Day_Sunless.png", dev, devcontext, &g_srv);
	if (!sts) {
		MessageBox(nullptr, "CreateSRVfromFile error(MeshWallInit)", "error", MB_OK);
	}

	// 使用するシェーダーを生成
	g_shadersky.SetShader(
		"shader/unlitTextureVS.hlsl",				// 頂点シェーダ
		"shader/unlitTexturePS.hlsl");				// ピクセルシェーダ

	// 壁メッシュ生成
	g_skymesh.Init(
		1, 1,					// 分割数
		skySize,					// サイズ
		skySize,					// サイズ
		Color(1, 1, 1, 1),		// 頂点カラー
		Vector3(0, 0, 1));		// 法線ベクトル

	// メッシュレンダラ生成（壁用）
	g_meshrenderer.Init(g_skymesh);

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

// 壁描画
void DrawSky() {

	ID3D11Device* dev;
	ID3D11DeviceContext* devcontext;

	dev = Renderer::GetDevice();
	devcontext = Renderer::GetDeviceContext();

	g_shadersky.SetGPU();

	g_material.SetGPU();

	devcontext->PSSetShaderResources(0, 1, g_srv.GetAddressOf());

	for (int i = 0; i < 5; i++) {

		// ワールド変換行列生成
		Matrix mtx;
		DX11MakeWorldMatrix(
			mtx,
			g_scale,							// スケール
			g_rotation[i],						// 姿勢
			g_position[i]);						// 位置

		// GPUに行列をセットする
		Renderer::SetWorldMatrix(&mtx);

		g_meshrenderer.Draw();
	}
}