#include	"TrainingRoomFloor.h"
#include	"Player.h"
#include	"CPlane.h"
#include	"Collision.h"
#include	"Field.h"

using namespace DirectX::SimpleMath;

// 床用
static CShader		g_shaderfield;
//  床メッシュ
static CUndulationPlaneMesh g_planemesh;
// 床メッシュ（レンダリングデータ）
static CMeshRenderer g_meshrenderer;
// 床用マテリアル
static CMaterial	g_material;
// SRV
static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_srv;

std::vector<CPlane> t_planes;

TRoom g_troom;

// プレイヤの情報(S,R,T)---------------------------------------------------------------

static DirectX::XMFLOAT3	g_position = { 0.0f,0.0f,0.0f };		// 位置
static DirectX::XMFLOAT3	g_rotation = { 0.0f,0.0f,0.0f };		// 姿勢
static DirectX::XMFLOAT3	g_scale = { 1.0f,1.0f,1.0f };			// 拡縮

//-------------------------------------------------------------------------------------
// 現在位置の高さを求める
float  GetTRoomHeight(Vector3 pos) {
	static bool firstflag = true;
	if (firstflag == true) {
		makepalenequatation();
		firstflag = false;
	}

	float t;
	// 面数分
	for (unsigned int idx = 0; idx < t_planes.size(); idx++) {
		Vector3 up = { 0,1,0 };
		Vector3 startpoint = { pos.x,0,pos.z };
		Plane p = t_planes[idx].GetPlaneInfo().plane;
		Vector3 ans;

		bool sts = LinetoPlaneCross(p, startpoint, up, t, ans);
		if (sts) {
			sts = CheckInTriangle(
				t_planes[idx].GetPlaneInfo().p0,
				t_planes[idx].GetPlaneInfo().p1,
				t_planes[idx].GetPlaneInfo().p2, ans);
			if (sts) return ans.y;
		}
	}

	return 0;
}

// 初期化
void InitTRoom() {
	// 使用するシェーダーを生成
	g_shaderfield.SetShader(
		"shader/unlitTextureVS.hlsl",				// 頂点シェーダ
		"shader/unlitTexturePS.hlsl");				// ピクセルシェーダ

	// 床メッシュ生成
	g_planemesh.Init(
		1, 1,					// 分割数
		g_troom.TRoomX_MAX,		// サイズ
		g_troom.TRoomZ_MAX,		// サイズ
		Color(1, 1, 1, 1),		// 頂点カラー
		Vector3(0, 0, -1),		// 法線ベクトルz
		true);					// XZ平面

	// 凸凹を付ける（一様分布で）
	g_planemesh.MakeUndulation(
		0.0f,					// 最小
		0.0f);					// 最大

	// メッシュレンダラ生成（床用）
	g_meshrenderer.Init(g_planemesh);

	ID3D11Device* dev;
	ID3D11DeviceContext* devcontext;

	dev = Renderer::GetDevice();
	devcontext = Renderer::GetDeviceContext();

	bool sts = CreateSRVfromFile("assets/texture/tatami.jpg", dev, devcontext, &g_srv);
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
}

// 描画
void DrawTRoom() {

	// 床用シェーダセット
	g_shaderfield.SetGPU();

	// 床用マテリアルセット
	g_material.SetGPU();

	// テクスチャをセット
	ID3D11DeviceContext* devcontext;
	devcontext = Renderer::GetDeviceContext();
	devcontext->PSSetShaderResources(0, 1, g_srv.GetAddressOf());

	// GPUに行列をセットする
	Matrix mtx;
	DX11MakeWorldMatrix(
		mtx,
		g_scale,							// スケール
		g_rotation,							// 姿勢
		g_position);						// 位置

	Renderer::SetWorldMatrix(&mtx);

	// 床描画
	g_meshrenderer.Draw();
}
