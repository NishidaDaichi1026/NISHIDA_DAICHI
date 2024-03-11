#include	<random>
#include	"Magma_Rock.h"
#include	"CMagma_Rock.h"
#include	"CSphereMesh.h"
#include	"renderer.h"
#include	"CBoundingSphere.h"
#include	"Field.h"

using namespace DirectX::SimpleMath;
//敵の最大数
constexpr unsigned int Magma_MAX = 10;
// エネミー群
static std::vector<CMagma>	g_MRocks;

static std::mt19937		g_rnd;				// メルセンヌツイスター

Field m_field;

//敵の情報取得-----------------------------------------------
// 位置取得
DirectX::XMFLOAT3 GetPositionMagma(int idx) {
	return g_MRocks[idx].GetPosition();
}
// 姿勢取得
DirectX::XMFLOAT3 GetRotationMagma(int idx) {
	return g_MRocks[idx].GetRotation();
}
// 移動量取得
DirectX::XMFLOAT3 GetMoveMagma(int idx) {
	return g_MRocks[idx].GetMove();
}
//　敵の最大数取得
int MagmaMax() {
	return Magma_MAX;
}
//-----------------------------------------------------------

// 初期化
void InitMagma()
{
	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// 頂点シェーダ
		"shader/vertexLightingPS.hlsl");				// ピクセルシェーダ

	// モデルファイル名
	std::string filename[] = {
		u8"assets\\model\\マグマ岩\\magma_rock.obj",
	};

	// メッシュ生成（ジオメトリデータ）
	g_staticmesh.Init(filename[0]);

	// 描画の為のデータ生成
	g_staticmeshrenderer.Init(g_staticmesh);

	//// エネミーリサイズ
	g_MRocks.resize(Magma_MAX);

	InitMagmaState();

	// 初期化
	for (auto& m : g_MRocks) {
		m.SetShader(&g_shader);
		m.SetMesh(&g_staticmesh);
		m.SetMeshRenderer(&g_staticmeshrenderer);
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
void InitMagmaState() {

	std::random_device rnddev;			// 真の乱数

	g_rnd.seed(rnddev());				// シード値セット

	// 乱数分布器
	std::uniform_real_distribution<float>	distx(-m_field.FieldX_MAX / 3, m_field.FieldX_MAX / 3);
	std::uniform_real_distribution<float>	distz(-m_field.FieldZ_MAX / 3, m_field.FieldZ_MAX / 3);
	std::uniform_real_distribution<float>	disty(0.0f, 0.0f);

	for (auto& m : g_MRocks) {
		Vector3 Pos{};
		Pos.x = distx(g_rnd);
		Pos.y = disty(g_rnd);
		Pos.z = distz(g_rnd);
		m.InitMagma_ST();
		m.SetPosition(Pos);
	}
}

// 更新
void UpdateMagma()
{
	for (auto& m : g_MRocks) {
		m.Update();
	}

}

// 描画
void DrawMagma()
{
	// 敵描画
	for (auto& m : g_MRocks) {
		m.Draw();
	}
}

// 終了処理
void ExitMagma() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}