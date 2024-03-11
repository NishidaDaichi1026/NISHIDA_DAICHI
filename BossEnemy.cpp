#include	"BossEnemy.h"
#include	"Field.h"
#include	"BossWeapon.h"
#include	"State.h"
#include	"XAudio2.h"

//↓当たり判定に使用
#include	"Player.h"
#include	"weapon.h"
#include	"FireBall.h"
#include	"NormalStar.h"

using namespace DirectX::SimpleMath;

// プレイヤの情報(S,R,T)
static Vector3	g_position = { 0.0f,0.0f,200.0f };		// 位置
static Vector3	g_rotation = { 0.0f,0.0f,0.0f };		// 姿勢
static Vector3	g_scale = { 0.2f,0.2f,0.2f };			// 拡縮

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
DirectX::XMFLOAT3 bossTrans(0, 0, 0);

DirectX::SimpleMath::Matrix		b_mtx;

const	float RATE_ROTATE_BOSS = 0.05f;

Boss_ST boss_st;
BossAbility B_abi;

Field bs_field;

//ステータスの取得
extern ST state[];
extern ATTRIBUTE attr[];

//-----------------------------------------------
//ボスの情報を取得
Boss_ST* GetBossST() {
	return &boss_st;
}
BossAbility* GetBossABI() {
	return &B_abi;
}
DirectX::XMFLOAT3 GetBossPosition() {
	return g_position;
}
DirectX::XMFLOAT3 GetBossRotation() {
	return g_rotation;
}
DirectX::XMFLOAT3 GetBossScale() {
	return g_scale;
}
DirectX::XMFLOAT3 GetBossMove() {
	return g_move;
}
DirectX::SimpleMath::Matrix GetBossMatrix() {
	return b_mtx;
}
//-----------------------------------------------

BoundingSphere GetBossBS() {

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

void InitBoss()
{
	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// 頂点シェーダ
		"shader/vertexLightingPS.hlsl");				// ピクセルシェーダ

	// モデルファイル名
	std::string filename[] = {
		u8"assets\\model\\mino.fbx",
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

	InitBossWeapon();
	InitBoss_ST();
}

void InitBoss_ST() {

	boss_st.B_sts = boss_st.Live;
	//各値はState.cpp参照
	//基本データ
	B_abi.ID = state[3].ID;
	B_abi.name = state[B_abi.ID].name;
	//ステータス決定
	B_abi.HP = state[B_abi.ID].HP;
	B_abi.ATTACK = state[B_abi.ID].ATTACK;
	B_abi.DEFENCE = state[B_abi.ID].DEFENCE;
	B_abi.SPEED = state[B_abi.ID].SPEED;

	//属性決定
	B_abi.A_ID = attr[0].ID;
	B_abi.attrName = attr[B_abi.A_ID].attrName;
	B_abi.normal = attr[B_abi.A_ID].normal;
	B_abi.fire = attr[B_abi.A_ID].fire;
	B_abi.thunder = attr[B_abi.A_ID].thunder;
	B_abi.water = attr[B_abi.A_ID].water;
	B_abi.ice = attr[B_abi.A_ID].ice;

	//初期位置
	g_position = { 0.0f,0.0f,200.0f };
	g_rotation = { 0.0f,0.0f,0.0f };
}

void ExitBoss() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
	ExitBossWeapon();
}

void UpdateBoss() {

	BossBattle();

	Player* g_player = GetPlayer();
	if (g_player->ULT_I == 1)return;		//氷のスキルを発動したら動かなくする(凍る)
	
	//ボスの武器更新
	UpdateBossWeapon();

	// プレイヤの座標を取得
	DirectX::SimpleMath::Vector3 playerpos = GetPositionPlayer();

	// atan2を使用して角度を求める
	g_destrot.y = atan2f(-(playerpos.x - g_position.x), -(playerpos.z - g_position.z));

	// 現在の向きとの差分を計算する
	float fDiffRotY = g_destrot.y - g_rotation.y;

	//プレイヤーとの距離を取得
	boss_st.FromPlayerLength = (playerpos - g_position).Length();

	//ダメージ時ノックバック
		//ノックバック方向を固定するために、ダメージ時にg_moveを更新しない
	if (boss_st.DAMAGE_CHACK == 1) {
		g_position -= (g_move / boss_st.FromPlayerLength) * boss_st.KnockBackPower;
	}
	else {
		//移動量取得
		g_move = playerpos - g_position;
	}

	if (boss_st.FromPlayerLength <= boss_st.S_Int_Leangth)return;
	g_position += (g_move / boss_st.FromPlayerLength) * B_abi.SPEED;

	// atan2を使用して角度を求める
	g_destrot.y = atan2f(-(playerpos.x - g_position.x), -(playerpos.z - g_position.z));

	// 補正（－１８０～１８０の範囲）
	if (fDiffRotY > DirectX::XM_PI)
	{
		fDiffRotY -= DirectX::XM_PI * 2.0f;
	}
	if (fDiffRotY < -DirectX::XM_PI)
	{
		fDiffRotY += DirectX::XM_PI * 2.0f;
	}

	// 回転角度計算
	g_rotation.y += fDiffRotY * RATE_ROTATE_BOSS;
	if (g_rotation.y > DirectX::XM_PI)
	{
		g_rotation.y -= DirectX::XM_PI * 2.0f;
	}
	if (g_rotation.y < -DirectX::XM_PI)
	{
		g_rotation.y += DirectX::XM_PI * 2.0f;
	}


	// 範囲チェック
	if (g_position.x <= -bs_field.FieldX_MAX / 2.5f) {
		g_position.x = -bs_field.FieldX_MAX / 2.5f;
	}
	if (g_position.x >= bs_field.FieldX_MAX / 2.5f) {
		g_position.x = bs_field.FieldX_MAX / 2.5f;
	}
	if (g_position.z >= bs_field.FieldZ_MAX / 2.5f) {
		g_position.z = bs_field.FieldZ_MAX / 2.5f;
	}
	if (g_position.z <= -bs_field.FieldZ_MAX / 2.5f) {
		g_position.z = -bs_field.FieldZ_MAX / 2.5f;
	}
	
}

void BossBattle() {
	float height;
	// 高さを取得
	height = GetFieldHeight(g_position);
	g_position.y = height + g_scale.y;

	Player* g_player = GetPlayer();
	Weapon_ST* weapon_st = GetWeaponST();
	HitStop* h_stop = GetHitStop();

	// プレイヤの攻撃に使用するBSを取得
	BoundingSphere pbs = GetWeaponBS();
	BoundingSphere fbs = GetFireBallBS();
	BoundingSphere sbs = GetNormalStarBS();

	//プレイヤーの攻撃ヒット判定
	bool sts = HitCheckSphere(pbs, GetBossBS());
	if (sts && boss_st.DAMAGE_CHACK == 0 && weapon_st->WeaponHitCheck == 1) {
		BossDamageHitCalc();
		boss_st.DAMAGE_CHACK = 1;
		h_stop->hitchack = true;
		PlaySound(SE7);
	}
	//一定時間ダメージを受けない
	if (boss_st.DAMAGE_CHACK == 1) {
		boss_st.d_HitInterval++;
		if (boss_st.d_HitInterval >= boss_st.NodamageTime) {
			boss_st.DAMAGE_CHACK = 0;
			boss_st.d_HitInterval = 0;
		}
	}

	//火の必殺技に対する処理
	bool usts = HitCheckSphere(fbs, GetBossBS());
	if (g_player->ULT_F == 1) {
		boss_st.ULT_HIT_INTERVAL++;
		if (usts && boss_st.ULT_HIT_INTERVAL >= boss_st.NodamageTime) {
			BossULTDamageHitCalc();
			h_stop->hitchack = true;
			boss_st.ULT_HIT_INTERVAL = 0;
		}
	}
	//無属性の必殺技に対する処理
	bool nssts = HitCheckSphere(sbs, GetBossBS());
	if (g_player->ULT_N == 1) {
		B_abi.DEFENCE /= 10;			//当たっていると防御力減
	}
	else {
		B_abi.DEFENCE = state[B_abi.ID].DEFENCE;	//当っていないと初期値を入れる
	}

	//雷属性の必殺技に対する処理（仮）
	if (g_player->ULT_T == 1 && boss_st.DAMAGE_CHACK == 0) {
		boss_st.DAMAGE_CHACK = 1;
		BossULTDamageHitCalc();
		h_stop->hitchack = true;
	}
	//敵を倒す処理
	if (B_abi.HP <= 0) {
		boss_st.B_sts = boss_st.Dead;
	}
}

//プレイヤーの通常攻撃の計算
void BossDamageHitCalc() {
	PlayerAbility* P_abi = GetPlayerAbi();
	Weapon_ST* weapon_st = GetWeaponST();
	//プレイヤーの攻撃力とエネミーの防御力からダメージを計算
	P_abi->DAMAGE = P_abi->ATTACK - B_abi.DEFENCE;
	//プレイヤーの属性と応じた属性耐性の倍率をダメージに掛ける
	switch (P_abi->A_ID) {
	case 0:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.normal;
		break;
	case 1:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.fire;
		break;
	case 2:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.water;
		break;
	case 3:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.ice;
		break;
	case 4:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.thunder;
		break;
	default:
		break;
	}

	//チャージ出来ていたらダメージに倍率
	if (weapon_st->Max_charge == true) {
		P_abi->DAMAGE *= weapon_st->chargeMag;
	}

	//ダメージを与える
	B_abi.HP = B_abi.HP - P_abi->DAMAGE;
}

//プレイヤーの必殺技の計算
void BossULTDamageHitCalc() {
	PlayerAbility* P_abi = GetPlayerAbi();
	//プレイヤーの攻撃力とエネミーの防御力からダメージを計算
	P_abi->DAMAGE = P_abi->ATTACK - B_abi.DEFENCE;
	//プレイヤーの属性と応じた属性耐性の倍率をダメージに掛ける
	switch (P_abi->A_ID) {
	case 0:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.normal;
		break;
	case 1:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.fire;
		break;
	case 2:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.water;
		break;
	case 3:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.ice;
		break;
	case 4:
		P_abi->DAMAGE = P_abi->DAMAGE * B_abi.thunder;
		break;
	default:
		break;
	}
	P_abi->DAMAGE *= 2;
	//ダメージを与える
	B_abi.HP = B_abi.HP - P_abi->DAMAGE;
}

void DrawBoss()
{
	//死んでいたら描画しない
	if (boss_st.B_sts == boss_st.Dead)return;
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

	//ボスの武器表示
	DrawBossWeapon();
}