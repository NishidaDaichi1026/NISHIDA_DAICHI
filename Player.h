#pragma once
#include	<directxmath.h>
#include	"3DModel.h"
#include	"CPlaneMesh.h"
#include	"CMeshRenderer.h"
#include	"CBoundingSphere.h"
#include	"CSphereMesh.h"
#include	"Field.h"
#include	"TrainingRoomFloor.h"

constexpr	float VALUE_MOVE_MODEL = 0.5f;						// 移動スピード
constexpr	float VALUE_ROTATE_MODEL = DirectX::XM_PI * 0.02f;	// 回転スピード
constexpr	float RATE_ROTATE_MODEL = 0.40f;					// 回転慣性係数
constexpr	float RATE_MOVE_MODEL = 0.20f;						// 移動慣性係数

enum class PlayerState {
	LIVE,		//生きている
	DEAD		//死んでいる
};

struct Player{
	PlayerState state = PlayerState::LIVE;
	float PossibleTime = 5.0;			//スキルクールタイム
	float P_sec = 0.0f;					//スキルのクールタイムに使用
	clock_t start = timeGetTime();		//スキルのクールタイムに使用
	float playerlimit = 20.0f;			//範囲取得
	bool ThunderSkillChack = false;		//雷のスキルで画面を揺らすので使ったかのチェック
	//属性ID-----------------------
	const int NORMAL_ID = 0;
	const int FIRE_ID = 1;
	const int WATER_ID = 2;
	const int ICE_ID = 3;
	const int THUNDER_ID = 4;
	//チャージ変数-----------------
	int charge_F = 0;
	int charge_N = 0;
	int charge_W = 0;
	int charge_I = 0;
	int charge_T = 0;
	//必要チャージ数---------------
	int NeedCharge = 3;
	//スキルフラグ-----------------
	int ULT_F = 0;
	int ULT_N = 0;
	int ULT_W = 0;
	int ULT_I = 0;
	int ULT_T = 0;
	//スキル時間-------------------
	int F_Time = 0;
	int N_Time = 0;
	int W_Time = 0;
	int I_Time = 0;
	int T_Time = 0;
	//スキル継続時間---------------
	int SkillDuration = 10 * 60;	//時間＊フレーム
};

struct PlayerAbility
{
	//基本データ
	int ID;
	const char* name;
	//ステータス
	int HP;
	int ATTACK;
	int DEFENCE;
	float SPEED;
	int DAMAGE;
	//属性
	int A_ID;
	const char* attrName;
	//属性相性
	float normal;
	float fire;
	float water;
	float ice;
	float thunder;
};

// プロトタイプ宣言----------------------------------------

void InitPlayer();				//初期化
void PlayerULT();				//必殺技のモデルの初期化
void InitPlayerState();			//ステータス初期化
void UpdatePlayer();			//更新
void MovePlayer();				//移動
void UpdatePlayerBattle();		//バトルの処理（必殺技）
void PlayerAbiChange();			//属性の変更処理
void DrawPlayer();				//描画
void ExitPlayer();				//メモリ解放
//情報取得-------------------------------------------------
// 位置取得
DirectX::XMFLOAT3 GetPositionPlayer();
// 姿勢取得
DirectX::XMFLOAT3 GetRotationPlayer();
// 移動量取得
DirectX::XMFLOAT3 GetMovePlayer();
// プレイヤBS取得
BoundingSphere GetPlayerBS();

PlayerAbility* GetPlayerAbi();
Player* GetPlayer();
//外部からセット-------------------------------------------
void SetPlayerPosition(DirectX::SimpleMath::Vector3 pos);
void SetPlayerRotation(DirectX::SimpleMath::Vector3 rot);