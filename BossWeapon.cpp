#include	"BossWeapon.h"
#include	"BossEnemy.h"

#include	"Player.h"
#include	"XAudio2.h"

using namespace DirectX::SimpleMath;

// プレイヤの情報(S,R,T)
static Vector3	g_position = { 0.0f,0.0f,200.0f };		// 位置
static Vector3	g_rotation = { 0.0f,0.0f,0.0f };		// 姿勢
static Vector3	g_scale = { 0.2f,0.2f,0.4f };			// 拡縮

// Bounding sphere
static CBoundingSphere g_bs;
// 球
static CSphereMesh g_sphere;
// メッシュレンダラ
static CMeshRenderer g_meshrenderer;
// マテリアル
static CMaterial g_material;

// 描画の為に必要な情報
// 使用するシェーダー
static CShader		g_shader;

// スタティックメッシュ（ジオメトリデータ）
static CStaticMesh g_staticmesh;

// メッシュレンダラー
static CStaticMeshRenderer g_staticmeshrenderer;

static Vector3	g_move = { 0.0f,0.0f,0.0f };			// 移動量
static Vector3	g_destrot = { 0.0f,0.0f,0.0f };			// 目標回転角度
DirectX::XMFLOAT3 bwTrans(0, 0, 0);


DirectX::SimpleMath::Matrix		g_mtx;

BossWeapon_ST bossweapon_st;

//ボスの武器の情報取得-------------------------------------
BossWeapon_ST* GetBossWeaponST() {
	return &bossweapon_st;
}
DirectX::SimpleMath::Vector3 GetBWPosition() {
	return g_position;
}
DirectX::SimpleMath::Vector3 GetBWScale() {
	return g_scale;
}
DirectX::SimpleMath::Vector3 GetBWMove() {
	return g_move;
}
DirectX::SimpleMath::Vector3 GetBWRotation() {
	return g_rotation;;
}
DirectX::SimpleMath::Matrix GetBWMatrix() {
	return g_mtx;
}
//---------------------------------------------------------
BoundingSphere GetBossWeaponBS() {

	// ワールド変換行列生成
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_scale,							// スケール
		g_rotation,							// 姿勢
		g_position);						// 位置

	// BS生成
	BoundingSphere bs;
	bs = g_bs.MakeBS(mtx, g_scale);

	return bs;
}

void InitBossWeapon()
{
	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// 頂点シェーダ
		"shader/vertexLightingPS.hlsl");				// ピクセルシェーダ

	// モデルファイル名
	std::string filename[] = {
		u8"assets\\model\\ax.fbx",
	};

	// メッシュ生成（ジオメトリデータ)
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

void ExitBossWeapon() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}

//以下ボスの攻撃に使用-------------------------------------
void UpdateBossWeapon() {

	g_position = GetBossPosition();

	g_rotation = GetBossRotation();
	
	g_move = GetBossMove();

	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, g_scale, g_rotation, bwTrans);

	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;

	g_position = g_position + zAxis * 5.0f;
	g_position.y = g_position.y;

	Vector3 rAxis;
	rAxis.x = mtx._21;
	rAxis.y = mtx._22;
	rAxis.z = mtx._23;

	g_rotation = g_rotation + rAxis;

	BoundingSphere pbs = GetPlayerBS();
	Boss_ST* b_st = GetBossST();
	
	//ボスの攻撃ヒット判定
	bool sts = HitCheckSphere(pbs, GetBossWeaponBS());

	//一定距離内にプレイヤーがいれば攻撃フラグを立てる
	//余裕を持たせるため＋１０
	if (b_st->FromPlayerLength <= b_st->S_Int_Leangth + 10) {
		//前の攻撃から一定時間空いていれば
		if (bossweapon_st.BossAIChack >= bossweapon_st.BossAttInterval) {
			PlaySound(SE1);
			bossweapon_st.attcheck = 1;
			bossweapon_st.WeaponHitCheck = 1;
		}
	}
	//攻撃がヒットしたらダメージ計算
	if (sts && bossweapon_st.WeaponHitCheck == 1) {
		BossDamageCalc();
		PlaySound(SE7);
		bossweapon_st.WeaponHitCheck = 0;
	}
	//攻撃アニメーション
	if (bossweapon_st.attcheck == 1) {
		g_position = g_position - zAxis * 10.0f;
		g_position.y = g_position.y + 10.0f;

		g_rotation.x -= (bossweapon_st.attTime - 20) * 0.05f;
		g_rotation.y -= (bossweapon_st.attTime - 20) * 0.05f;

		bossweapon_st.attTime++;
	}
	//攻撃アニメーション終了
	if (bossweapon_st.attTime >= bossweapon_st.AttAnimeTime) {
		bossweapon_st.attcheck = 0;
		bossweapon_st.attTime = 0;
	}

	if (bossweapon_st.attcheck == 0) {		//攻撃のアニメーションをしていなければ
		bossweapon_st.BossAIChack++;		//攻撃間隔を取る
	}
	else
	{
		bossweapon_st.BossAIChack = 0;
	}
}

void BossDamageCalc() {
	
	BossAbility* b_abi = GetBossABI();
	PlayerAbility* p_abi = GetPlayerAbi();

	b_abi->DAMAGE = b_abi->ATTACK - p_abi->DEFENCE;
	p_abi->HP = p_abi->HP - b_abi->DAMAGE;

}
//---------------------------------------------------------
void DrawBossWeapon()
{
	// デバイスコンテキスト取得
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// ワールド変換行列生成
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_scale,							// スケール
		g_rotation,							// 姿勢
		g_position);						// 位置

	// GPUに行列をセットする
	Renderer::SetWorldMatrix(&mtx);

	// シェーダーをGPUにセット
	g_shader.SetGPU();

	// モデル描画
	g_staticmeshrenderer.Draw();

}