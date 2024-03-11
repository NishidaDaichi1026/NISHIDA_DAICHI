#include	"NormalStar.h"
#include	"Player.h"
//---------------------------------------------
//			無属性の必殺技のモデル
//---------------------------------------------

using namespace DirectX::SimpleMath;

// 描画の為に必要な情報
// 使用するシェーダー
static CShader		g_shader;

// スタティックメッシュ（ジオメトリデータ）
static CStaticMesh g_staticmesh;

// メッシュレンダラー
static CStaticMeshRenderer g_staticmeshrenderer;

// Bounding sphere
static CBoundingSphere g_bs;
// 球
static CSphereMesh g_sphere;
// メッシュレンダラ
static CMeshRenderer g_meshrenderer;
// マテリアル
static CMaterial g_material;

DirectX::XMFLOAT3 nsTrans(0, 0, 0);

CNormalStar g_normal_star;

static float rot_sp = 0.0f;				//回転速度

CNormalStar* GetNormalStar() {
	return &g_normal_star;
}

BoundingSphere GetNormalStarBS() {

	// ワールド変換行列生成
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_normal_star.scale,							// スケール
		g_normal_star.rotation,						// 姿勢
		g_normal_star.position);						// 位置

	// BS生成
	BoundingSphere bs;
	bs = g_bs.MakeBS(mtx, g_normal_star.scale);

	return bs;
}
//初期化
void CNormalStar::Init()
{
	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// 頂点シェーダ
		"shader/vertexLightingPS.hlsl");				// ピクセルシェーダ

	// モデルファイル名
	std::string filename[] = {
		u8"assets\\model\\HOSHI.obj",
	};

	// メッシュ生成（ジオメトリデータ）
	g_staticmesh.Init(filename[0]);
	// 描画の為のデータ生成
	g_staticmeshrenderer.Init(g_staticmesh);

	// 境界球を求める
	g_bs.Caliculate(g_staticmesh.GetVertices());

	g_sphere.Init(g_bs.GetRadius(), Color(1, 1, 1, 1), 100, 100);
	g_meshrenderer.Init(g_sphere);

	MATERIAL mtrl;
	mtrl.Ambient = Color(0, 0, 0, 0);
	mtrl.Diffuse = Color(1, 1, 1, 0.3f);
	mtrl.Specular = Color(0, 0, 0, 0);
	mtrl.Shininess = 0;
	mtrl.Emission = Color(0, 0, 0, 0);
	mtrl.TextureEnable = FALSE;

	g_material.Init(mtrl);
}
//メモリ解放
void CNormalStar::Exit() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}
//更新
void CNormalStar::Update() {
	//回転
	rot_sp += 0.005f;

	g_normal_star.position = GetPositionPlayer();

	g_normal_star.rotation.y = rot_sp;

	g_normal_star.move = GetMovePlayer();				// プレイヤの移動量取得

	Player* player = GetPlayer();

	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, g_normal_star.scale, g_normal_star.rotation, nsTrans);

	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;

	g_normal_star.position = g_normal_star.position + zAxis;

	//-----------------------------------------------------------------------------------------------
	//大きくするアニメーション
	if (player->N_Time <= 120) {
		g_normal_star.scale.x = g_normal_star.scale.y = g_normal_star.scale.z += 0.005f;
	}
	else {
		if (g_normal_star.scale.x <= 30.0f) {
			g_normal_star.scale.x = g_normal_star.scale.y = g_normal_star.scale.z += 0.5f;
		}
	}
	//終了時元に戻す
	if (player->N_Time == 0) {
		g_normal_star.scale.x = g_normal_star.scale.y = g_normal_star.scale.z = 0.0f;
	}
	//-----------------------------------------------------------------------------------------------
}
//描画
void CNormalStar::Draw(){

	// デバイスコンテキスト取得
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// ワールド変換行列生成
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_normal_star.scale,							// スケール
		g_normal_star.rotation,							// 姿勢
		g_normal_star.position);						// 位置

	// GPUに行列をセットする
	Renderer::SetWorldMatrix(&mtx);

	// シェーダーをGPUにセット
	g_shader.SetGPU();

	// モデル描画
	g_staticmeshrenderer.Draw();
	
}