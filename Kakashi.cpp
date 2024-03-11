#include	"CSphereMesh.h"
#include	"renderer.h"
#include	"CBoundingSphere.h"
#include	"Field.h"
#include	"CKakashi.h"
#include	"Kakashi.h"

using namespace DirectX::SimpleMath;
//敵の最大数
constexpr unsigned int Kakashi_MAX = 5;
// エネミー群
static std::vector<CKAKASHI>	g_kakashi;

static std::mt19937		g_rnd;				// メルセンヌツイスター

Field k_field;

// 初期化
void InitKakashi()
{
	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// 頂点シェーダ
		"shader/vertexLightingPS.hlsl");				// ピクセルシェーダ

	// モデルファイル名
	std::string filename[] = {
		u8"assets\\model\\kakashi.fbx",
	};

	// メッシュ生成（ジオメトリデータ）
	g_staticmesh.Init(filename[0]);

	// 描画の為のデータ生成
	g_staticmeshrenderer.Init(g_staticmesh);

	//// エネミーリサイズ
	g_kakashi.resize(Kakashi_MAX);

	InitKakashiState();

	// 初期化
	for (auto& m : g_kakashi) {
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
void InitKakashiState() {
	int i = 0;
	for (auto& m : g_kakashi) {
		Vector3 Pos{};
		Pos.x = (i * 75)-150;
		Pos.y = 0;
		Pos.z = 0;
		m.SetA_ID(i);
		m.InitKakashi_ST();
		m.SetPosition(Pos);
		i++;
	}
}

// 更新
void UpdateKakashi()
{
	for (auto& m : g_kakashi) {
		m.Update();
	}

}

// 描画
void DrawKakashi()
{
	// 敵描画
	for (auto& m : g_kakashi) {
		m.Draw();
	}
}

// 終了処理
void ExitKakashi() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}