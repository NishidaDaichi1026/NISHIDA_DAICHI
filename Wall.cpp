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
constexpr	int MAX_MESH_WALL = 4;									// 壁の総数

static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_srv;
static Microsoft::WRL::ComPtr<ID3D11Resource> g_texres;

static Vector3 g_scale = { 1,1,1 };

static float wallSize = 1024.0f;	//サイズ

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

// 壁用
static CShader		g_shaderwall;
//  壁メッシュ（ジオメトリデータ）
static CPlaneMesh g_wallmesh;
// 壁メッシュ（レンダリングデータ）
static CMeshRenderer g_meshrenderer;
// 壁用マテリアル
static CMaterial	g_material;

// 初期化
void InitWall() {

	ID3D11Device* dev;
	ID3D11DeviceContext* devcontext;

	dev = Renderer::GetDevice();
	devcontext = Renderer::GetDeviceContext();

	bool sts = CreateSRVfromFile("assets/texture/wall000.jpg", dev, devcontext, &g_srv);
	if (!sts) {
		MessageBox(nullptr, "CreateSRVfromFile error(MeshWallInit)", "error", MB_OK);
	}

	// 使用するシェーダーを生成
	g_shaderwall.SetShader(
		"shader/unlitTextureVS.hlsl",				// 頂点シェーダ
		"shader/unlitTexturePS.hlsl");				// ピクセルシェーダ

	// 壁メッシュ生成
	g_wallmesh.Init(
		10, 10,					// 分割数
		wallSize,				// サイズ
		wallSize,				// サイズ
		Color(1, 1, 1, 1),		// 頂点カラー
		Vector3(0, 0, 1));		// 法線ベクトル

	// メッシュレンダラ生成（壁用）
	g_meshrenderer.Init(g_wallmesh);

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
void DrawWall() {

	ID3D11Device* dev;
	ID3D11DeviceContext* devcontext;

	dev = Renderer::GetDevice();
	devcontext = Renderer::GetDeviceContext();

	g_shaderwall.SetGPU();

	g_material.SetGPU();

	devcontext->PSSetShaderResources(0, 1, g_srv.GetAddressOf());

	for (int i = 0; i < 4; i++) {

		// 壁なのでX軸回転90度
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