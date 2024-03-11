#include	<random>
#include	"Slime.h"
#include	"CSlime.h"
#include	"CSphereMesh.h"
#include	"renderer.h"
#include	"CBoundingSphere.h"
#include	"Field.h"


using namespace DirectX::SimpleMath;
//敵の最大数
constexpr unsigned int SLIME_MAX = 10;
// エネミー群
static std::vector<CSLIME>	g_Slimes;

static std::mt19937		g_rnd;				// メルセンヌツイスター

Field e_field;

//----------------------------------------------------------------------------
// 情報取得

// 位置取得
DirectX::XMFLOAT3 GetPositionSlime(int idx) {
	return g_Slimes[idx].GetPosition();
}
// 姿勢取得
DirectX::XMFLOAT3 GetRotationSlime(int idx) {
	return g_Slimes[idx].GetRotation();
}
// 移動量取得
DirectX::XMFLOAT3 GetMoveSlime(int idx) {
	return g_Slimes[idx].GetMove();
}
//敵の最大数取得
int SlimeMax() {
	return SLIME_MAX;
}
//----------------------------------------------------------------------------

// 初期化
void InitSlime()
{
	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// 頂点シェーダ
		"shader/vertexLightingPS.hlsl");				// ピクセルシェーダ

	// モデルファイル名
	std::string filename[] = {
		u8"assets\\model\\マグマ岩\\magma_rock.obj",
		u8"assets\\model\\氷スライム\\氷スライム.obj"
	};

	// メッシュ生成（ジオメトリデータ）
	g_staticmesh.Init(filename[1]);

	// 描画の為のデータ生成
	g_staticmeshrenderer.Init(g_staticmesh);

	//// エネミーリサイズ
	g_Slimes.resize(SLIME_MAX);

	InitSlimeState();
	
	// 初期化
	for (auto& s : g_Slimes) {
		s.SetShader(&g_shader);
		s.SetMesh(&g_staticmesh);
		s.SetMeshRenderer(&g_staticmeshrenderer);
	}

	MATERIAL mtrl;
	mtrl.Ambient = Color(0, 0, 0, 0);
	mtrl.Diffuse = Color(1, 1, 1, 0.3f);
	mtrl.Specular = Color(0, 0, 0, 0);
	mtrl.Shininess = 0;
	mtrl.Emission = Color(0, 0, 0, 0);
	mtrl.TextureEnable = FALSE;

	g_material.Init(mtrl);

	// 境界球を求める
	g_bs.Caliculate(g_staticmesh.GetVertices());

	// 球メッシュを生成する
	g_sphere.Init(g_bs.GetRadius(), Color(1, 1, 1, 1), 100, 100);
	g_meshrenderer.Init(g_sphere);
}

//ステータス初期化
void InitSlimeState() {

	std::random_device rnddev;			// 真の乱数

	g_rnd.seed(rnddev());				// シード値セット

	// 乱数分布器
	std::uniform_real_distribution<float>	distx(-e_field.FieldX_MAX / 3, e_field.FieldX_MAX / 3);
	std::uniform_real_distribution<float>	distz(-e_field.FieldZ_MAX / 3, e_field.FieldZ_MAX / 3);
	std::uniform_real_distribution<float>	disty(0.0f, 0.0f);


	for (auto& s : g_Slimes) {
		Vector3 Pos{};
		Pos.x = distx(g_rnd);
		Pos.y = disty(g_rnd);
		Pos.z = distz(g_rnd);
		s.InitSlime_ST();
		s.SetPosition(Pos);
	}
}

// 更新
void UpdateSlime()
{
	for (auto& s : g_Slimes) {
		s.Update();
	}
	
}

// 描画
void DrawSlime()
{
	// 敵描画
	for (auto& s : g_Slimes) {
		s.Draw();
	}
}

// 終了処理
void ExitSlime() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}