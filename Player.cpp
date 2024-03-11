#include	"Player.h"
#include	"CBoundingSphere.h"
#include	"State.h"
#include	"FireBall.h"
#include	"NormalStar.h"
#include	"weapon.h"
#include	"sceneChange.h"
#include	"SnowParticle.h"
#include	"XAudio2.h"

using namespace DirectX::SimpleMath;

// プレイヤの情報(S,R,T)
static Vector3	g_position = { 0.0f,0.0f,0.0f };		// 位置
static Vector3	g_rotation = { 0.0f,0.0f,0.0f };		// 姿勢
static Vector3	g_scale = { 15.0f,15.0f,15.0f };		// 拡縮

// 描画の為に必要な情報
// 使用するシェーダー
static CShader		g_shader;

// スタティックメッシュ（ジオメトリデータ）
static CStaticMesh g_staticmesh;

// Bounding sphere
static CBoundingSphere g_bs;
// 球
static CSphereMesh g_sphere;
// メッシュレンダラ
static CMeshRenderer g_meshrenderer;
// マテリアル
static CMaterial g_material;

// メッシュレンダラー
static CStaticMeshRenderer g_staticmeshrenderer;

static Vector3  g_attackmove = { 0.0f,0.0f,0.0f };		//攻撃時の移動
static Vector3	g_move = { 0.0f,0.0f,0.0f };			//移動量
static Vector3  g_movechack = { 1.5f,1.5f,1.5f };		//移動しているかのチェック
static Vector3	g_destrot = { 0.0f,0.0f,0.0f };			// 目標回転角度

DirectX::XMFLOAT3 Playertrans(0, 0, 0);

//-----------------------------------------------------------
Player g_player;
PlayerAbility P_abi;
CFireBall fireball;
CNormalStar NStar;
Field p_field;
TRoom p_troom;
//-----------------------------------------------------------

//ステータスの取得
extern ST state[];
extern ATTRIBUTE attr[];

//-----------------------------------------------------------
// 位置取得
DirectX::XMFLOAT3 GetPositionPlayer() {
	return g_position;
}
// 姿勢取得
DirectX::XMFLOAT3 GetRotationPlayer() {
	return g_rotation;
}
// 移動量取得
DirectX::XMFLOAT3 GetMovePlayer() {
	return g_move;
}
//　情報取得
PlayerAbility* GetPlayerAbi() {
	return &P_abi;
}
Player* GetPlayer() {
	return &g_player;
}

//-----------------------------------------------------------
//セット
void SetPlayerPosition(DirectX::SimpleMath::Vector3 pos) {
	g_position = pos;
}
void SetPlayerRotation(DirectX::SimpleMath::Vector3 rot) {
	g_rotation = rot;
}
//-----------------------------------------------------------
BoundingSphere GetPlayerBS() {

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

// 初期化
void InitPlayer() {

	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// 頂点シェーダ
		"shader/vertexLightingPS.hlsl");				// ピクセルシェーダ

	// モデルファイル名
	std::string filename[] = {
		u8"assets\\model\\Player\\Player.fbx",
	};

	// メッシュ生成（ジオメトリデータ）
	g_staticmesh.Init(filename[0]);

	// 描画の為のデータ生成
	g_staticmeshrenderer.Init(g_staticmesh);

	InitPlayerState();

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

	PlayerULT();
	// パーティクル初期化
	InitSnowParticle();
}
//必殺技初期化
void PlayerULT() {
	fireball.Init();
	NStar.Init();
}
//ステータス初期化
void InitPlayerState() {
	//各値はState.cpp参照
	//基本データ
	P_abi.ID = state[0].ID;
	P_abi.name = state[P_abi.ID].name;
	//ステータス決定
	P_abi.HP = state[P_abi.ID].HP;
	P_abi.ATTACK = state[P_abi.ID].ATTACK;
	P_abi.DEFENCE = state[P_abi.ID].DEFENCE;
	P_abi.SPEED = state[P_abi.ID].SPEED;
	//属性決定
	P_abi.A_ID = attr[0].ID;
	P_abi.attrName = attr[P_abi.A_ID].attrName;
	P_abi.fire = attr[P_abi.A_ID].fire;
	P_abi.thunder = attr[P_abi.A_ID].thunder;
	P_abi.water = attr[P_abi.A_ID].water;
	P_abi.ice = attr[P_abi.A_ID].ice;

	//プレイヤーの情報
	g_position = { 0.0f,0.0f,0.0f };
	g_rotation = { 0.0f,0.0f,0.0f };

	g_player.charge_F = 0;
	g_player.charge_N = 0;
	g_player.charge_W = 0;
	g_player.charge_I = 0;
	g_player.charge_T = 0;

	g_player.ULT_F = 0;
	g_player.ULT_N = 0;
	g_player.ULT_W = 0;
	g_player.ULT_I = 0;
	g_player.ULT_T = 0;

	g_player.F_Time = 0;
	g_player.N_Time = 0;
	g_player.W_Time = 0;
	g_player.I_Time = 0;
	g_player.T_Time = 0;
}

// 更新
void UpdatePlayer() {
	// 高さを取得
	float height;
	
	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, g_scale, g_rotation, Playertrans);

	// 範囲チェック
	if (ReturnSceneNum() == 3) {
		//トレーニングルーム
		if (g_position.x <= -p_troom.TRoomX_MAX / 2 + g_player.playerlimit) {
			g_position.x = -p_troom.TRoomX_MAX / 2 + g_player.playerlimit;
		}
		if (g_position.x >= p_troom.TRoomX_MAX / 2 - g_player.playerlimit) {
			g_position.x = p_troom.TRoomX_MAX / 2 - g_player.playerlimit;
		}
		if (g_position.z >= p_troom.TRoomZ_MAX / 2 - g_player.playerlimit) {
			g_position.z = p_troom.TRoomZ_MAX / 2 - g_player.playerlimit;
		}
		if (g_position.z <= -p_troom.TRoomZ_MAX / 2 + g_player.playerlimit) {
			g_position.z = -p_troom.TRoomZ_MAX / 2 + g_player.playerlimit;
		}
		height = GetTRoomHeight(g_position);
	}
	else {
		//フィールド
		if (g_position.x <= -p_field.FieldX_MAX / 2 + g_player.playerlimit) {
			g_position.x = -p_field.FieldX_MAX / 2 + g_player.playerlimit;
		}
		if (g_position.x >= p_field.FieldX_MAX / 2 - g_player.playerlimit) {
			g_position.x = p_field.FieldX_MAX / 2 - g_player.playerlimit;
		}
		if (g_position.z >= p_field.FieldZ_MAX / 2 - g_player.playerlimit) {
			g_position.z = p_field.FieldZ_MAX / 2 - g_player.playerlimit;
		}
		if (g_position.z <= -p_field.FieldZ_MAX / 2 + g_player.playerlimit) {
			g_position.z = -p_field.FieldZ_MAX / 2 + g_player.playerlimit;
		}
		height = GetFieldHeight(g_position);
	}
	Weapon_ST* weapon_P = GetWeaponST();

	if (g_player.state != PlayerState::LIVE)return;

	//攻撃時に前進
	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;

	zAxis.x = g_position.x + zAxis.x;
	zAxis.z = g_position.z + zAxis.z;
	if (weapon_P->Max_charge == true) {
		g_attackmove = ((zAxis - g_position) / 10) * weapon_P->chargeAtMove;
	}
	else{
		g_attackmove = (zAxis - g_position) / 10;
	}

	if (weapon_P->attTime >= 1&& weapon_P->attTime <= 10){
		//xyzいずれかが動いていればダッシュしながら攻撃
		if (g_move.x >= g_movechack.x || g_move.y >= g_movechack.y || g_move.z >= g_movechack.z) {
			g_move.x -= g_attackmove.x;
			g_move.z -= g_attackmove.z;
		}
		if (g_move.x <= -g_movechack.x || g_move.y <= -g_movechack.y || g_move.z <= -g_movechack.z) {
			g_move.x -= g_attackmove.x;
			g_move.z -= g_attackmove.z;
		}
	}

	MovePlayer();
	// 位置移動
	g_position += g_move;
	g_position.y = height;

	// 移動量に慣性をかける(減速慣性)
	g_move += -g_move * RATE_MOVE_MODEL;
	
	UpdatePlayerBattle();
}
//移動
void MovePlayer() {

	DirectX::XMFLOAT3 rotCamera;			// カメラの姿勢を取得する（オイラー角）
	// カメラ音角度を取得
	rotCamera = Camera::GetRotCamera();

	if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_A)) {
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
		{// 左前移動

			float radian;
			radian = rotCamera.y + DirectX::XM_PI * 0.75f;

			g_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			g_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			g_destrot.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// 左後移動

			float radian;
			radian = rotCamera.y + DirectX::XM_PI * 0.25f;

			g_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			g_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			g_destrot.y = radian;
		}
		else
		{// 左移動

			float radian;
			radian = rotCamera.y + DirectX::XM_PI * 0.50f;

			g_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			g_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			g_destrot.y = radian;
		}
	}

	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_D))
	{
		if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W)) {
			// 右前移動

			float radian;
			radian = rotCamera.y - DirectX::XM_PI * 0.75f;

			g_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			g_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			g_destrot.y = radian;

		}
		else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
		{// 右後移動
			float radian;
			radian = rotCamera.y + -DirectX::XM_PI * 0.25f;

			g_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			g_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			g_destrot.y = radian;
		}
		else
		{// 右移動

			float radian;
			radian = rotCamera.y + -DirectX::XM_PI * 0.50f;

			g_move.x -= sinf(radian) * VALUE_MOVE_MODEL;
			g_move.z -= cosf(radian) * VALUE_MOVE_MODEL;

			// 目標角度をセット
			g_destrot.y = radian;
		}
	}
	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_W))
	{
		// 前移動
		g_move.x -= sinf(DirectX::XM_PI + rotCamera.y) * VALUE_MOVE_MODEL;
		g_move.z -= cosf(DirectX::XM_PI + rotCamera.y) * VALUE_MOVE_MODEL;

		// 目標角度をセット
		g_destrot.y = rotCamera.y + DirectX::XM_PI;
	}

	else if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_S))
	{
		// 後移動
		g_move.x -= sinf(rotCamera.y) * VALUE_MOVE_MODEL;
		g_move.z -= cosf(rotCamera.y) * VALUE_MOVE_MODEL;

		// 目標角度をセット
		g_destrot.y = rotCamera.y;;
	}

	// 目標角度と現在角度との差分を求める
	float diffrot = g_destrot.y - g_rotation.y;
	if (diffrot > DirectX::XM_PI)
	{
		diffrot -= DirectX::XM_PI * 2.0f;
	}
	if (diffrot < -DirectX::XM_PI)
	{
		diffrot += DirectX::XM_PI * 2.0f;
	}

	// 角度速度に慣性を付ける
	g_rotation.y += diffrot * RATE_ROTATE_MODEL;

	if (g_rotation.y > DirectX::XM_PI)
	{
		g_rotation.y -= DirectX::XM_PI * 2.0f;
	}
	if (g_rotation.y < -DirectX::XM_PI)
	{
		g_rotation.y += DirectX::XM_PI * 2.0f;
	}
}
//バトルの処理
void UpdatePlayerBattle() {

	PlayerAbiChange();
	//無属性の必殺技-----------------------------------------------------------------------------
	if (g_player.charge_N >= g_player.NeedCharge && P_abi.A_ID == g_player.NORMAL_ID &&
		CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_Q)) {
		g_player.ULT_N = 1;
		g_player.charge_N = 0;
		PlaySound(SE8);
	}

	if (g_player.ULT_N == 1) {
		NStar.Update();
		g_player.N_Time++;
		if (g_player.N_Time >= g_player.SkillDuration) {
			g_player.ULT_N = 0;
			g_player.N_Time = 0;
		}
	}
	//火属性の必殺技-----------------------------------------------------------------------------
	if (g_player.charge_F >= g_player.NeedCharge && P_abi.A_ID == g_player.FIRE_ID &&
		CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_Q)) {
		g_player.ULT_F = 1;
		g_player.charge_F = 0;
		PlaySound(SE8);
	}

	if (g_player.ULT_F == 1) {
		fireball.Update();
		g_player.F_Time++;
		if (g_player.F_Time >= g_player.SkillDuration) {
			g_player.ULT_F = 0;
			g_player.F_Time = 0;
		}
	}
	//水属性の必殺技-----------------------------------------------------------------------------
	if (g_player.charge_W >= g_player.NeedCharge && P_abi.A_ID == g_player.WATER_ID &&
		CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_Q)) {
		g_player.ULT_W = 1;
		g_player.charge_W = 0;
		PlaySound(SE8);
		PlaySound(SE4);
	}

	if (g_player.ULT_W == 1) {
		g_player.W_Time++;
		if (P_abi.HP < state[P_abi.ID].HP) {
			P_abi.HP += state[P_abi.ID].HP / 500;
		}
		if (g_player.W_Time >= g_player.SkillDuration) {
			StopSound(SE4);
			g_player.ULT_W = 0;
			g_player.W_Time = 0;
		}
	}
	//氷属性の必殺技-----------------------------------------------------------------------------
	if (g_player.charge_I >= g_player.NeedCharge && P_abi.A_ID == g_player.ICE_ID &&
		CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_Q)) {
		g_player.ULT_I = 1;
		g_player.charge_I = 0;
		PlaySound(SE8);
		PlaySound(SE4);
	}

	if (g_player.ULT_I == 1) {
		UpdateSnowParticle();
		g_player.I_Time++;
		if (g_player.I_Time >= g_player.SkillDuration) {
			StopSound(SE4);
			g_player.ULT_I = 0;
			g_player.I_Time = 0;
		}
	}

	//雷属性の必殺技（仮）-----------------------------------------------------------------------
	if (g_player.charge_T >= g_player.NeedCharge && P_abi.A_ID == g_player.THUNDER_ID &&
		CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_Q)) {
		g_player.ULT_T = 1;
		g_player.charge_T = 0;
		g_player.ThunderSkillChack = true;
		PlaySound(SE8);
		PlaySound(SE3);
	}

	if (g_player.ULT_T == 1) {
		g_player.T_Time++;
		if (g_player.T_Time >= 60.0f) {
			g_player.ULT_T = 0;
			g_player.T_Time = 0;
		}
	}
}

// 描画
void DrawPlayer() {

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

	if (g_player.ULT_F == 1) {
		fireball.Draw();
	}

	if (g_player.ULT_N == 1) {
		NStar.Draw();
	}

	if (g_player.ULT_I == 1) {
		DrawSnowParticle();
	}
}

 //メモリ解放
void ExitPlayer() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
	fireball.Exit();
	NStar.Exit();
}


//攻撃の属性を変更
//スキルゲージのチャージ
void PlayerAbiChange() {

	clock_t end = timeGetTime();
	
	//スキルのクールタイム計算
	if (g_player.P_sec <= g_player.PossibleTime) {
		//前回スキルを撃った時から逆算
		g_player.P_sec = (float)(end - g_player.start) / CLOCKS_PER_SEC;
	}
	//無属性の必殺技-----------------------------------------------------------------
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_1)) {
		PlaySound(SE9);
		P_abi.A_ID = g_player.NORMAL_ID;
		//
		if (g_player.P_sec < g_player.PossibleTime)return;

		if (g_player.charge_N < 3 && g_player.ULT_N == 0) {
			g_player.charge_N += 1;
			g_player.start = timeGetTime();
			g_player.P_sec = 0;		//クールタイム初期化
		}
	}
	//火属性の必殺技-----------------------------------------------------------------
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_2)) {
		PlaySound(SE9);
		P_abi.A_ID = g_player.FIRE_ID;

		if (g_player.P_sec < g_player.PossibleTime)return;

		if (g_player.charge_F < 3 && g_player.ULT_F == 0) {
			g_player.charge_F += 1;
			g_player.start = timeGetTime();
			g_player.P_sec = 0;		//クールタイム初期化
		}
	}
	//水属性の必殺技-----------------------------------------------------------------
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_3)) {
		PlaySound(SE9);
		P_abi.A_ID = g_player.WATER_ID;

		if (g_player.P_sec < g_player.PossibleTime)return;

		if (g_player.charge_W < 3 && g_player.ULT_W == 0) {
			g_player.charge_W += 1;
			g_player.start = timeGetTime();
			g_player.P_sec = 0;		//クールタイム初期化
		}
	}
	//氷属性の必殺技-----------------------------------------------------------------
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_4)) {
		PlaySound(SE9);
		P_abi.A_ID = g_player.ICE_ID;

		if (g_player.P_sec < g_player.PossibleTime)return;

		if (g_player.charge_I < 3 && g_player.ULT_I == 0) {
			g_player.charge_I += 1;
			g_player.start = timeGetTime();
			g_player.P_sec = 0;		//クールタイム初期化
		}
	}
	//雷属性の必殺技-----------------------------------------------------------------
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_5)) {
		PlaySound(SE9);
		P_abi.A_ID = g_player.THUNDER_ID;

		if (g_player.P_sec < g_player.PossibleTime)return;

		if (g_player.charge_T < 3 && g_player.ULT_T == 0) {
			g_player.charge_T += 1;
			g_player.start = timeGetTime();
			g_player.P_sec = 0;		//クールタイム初期化
		}
	}
	//-------------------------------------------------------------------------------
}