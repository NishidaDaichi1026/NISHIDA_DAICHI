#include	"FireBall.h"
#include	"Player.h"
//------------------------------------------------
//			炎の必殺技のモデル
//------------------------------------------------

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

DirectX::XMFLOAT3 poTrans(0, 0, 0);

CFireBall g_fireball;

//回転
static float rot_sp = 0.0f;


CFireBall* GetFireBall(){
	return &g_fireball;
}

BoundingSphere GetFireBallBS() {

	// ワールド変換行列生成
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_fireball.scale,							// スケール
		g_fireball.rotation,						// 姿勢
		g_fireball.position);						// 位置

	// BS生成
	BoundingSphere bs;
	bs = g_bs.MakeBS(mtx, g_fireball.scale);

	return bs;
}

void CFireBall::Init()
{
	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// 頂点シェーダ
		"shader/vertexLightingPS.hlsl");				// ピクセルシェーダ

	// モデルファイル名
	std::string filename[] = {
		u8"assets\\model\\fireball.obj",
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

void CFireBall::Exit(){
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}

void CFireBall::Update() {

	rot_sp += 0.05f;		//回転
	Player* player = GetPlayer();
	
	g_fireball.position = GetPositionPlayer();

	g_fireball.rotation.y = rot_sp;

	g_fireball.move = GetMovePlayer();				// プレイヤの移動量取得

	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, g_fireball.scale, g_fireball.rotation, poTrans);

	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;
	//位置決め
	g_fireball.position = g_fireball.position + zAxis;
	g_fireball.position.y = g_fireball.position.y + 7.0f;

}

void CFireBall::Draw()
{
	// デバイスコンテキスト取得
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// ワールド変換行列生成
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_fireball.scale,							// スケール
		g_fireball.rotation,						// 姿勢
		g_fireball.position);						// 位置

	// GPUに行列をセットする
	Renderer::SetWorldMatrix(&mtx);

	// シェーダーをGPUにセット
	g_shader.SetGPU();

	// モデル描画
	g_staticmeshrenderer.Draw();

}