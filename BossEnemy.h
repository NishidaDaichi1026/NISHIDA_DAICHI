#pragma once

#include	<DirectXMath.h>
#include	"3DModel.h"
#include	"CMeshRenderer.h"
#include	"CBoundingSphere.h"
#include	"CSphereMesh.h"

struct Boss_ST {
	//ボスのステータス----------
	//初期ステータス
	int B_sts = 1;
	//--------------------------
	int Live = 1;	//生きている
	int Dead = 2;	//死んでいる
	//--------------------------
	
	float FromPlayerLength = 0.0f;		//プレイヤーとの距離
	int S_Int_Leangth = 40;			//プレイヤーと重ならないように(数字以下まで近寄らない)
	int DAMAGE_CHACK = 0;			//ダメージを受けたかのチェック
	int ULT_HIT_INTERVAL = 0;		//必殺技のヒット間隔
	float KnockBackPower = 5.0f;	//ノックバックの距離(力)
	
	//ダメージの間隔--------------------
	float d_HitInterval = 0.0f;
	//秒数＊フレーム
	float NodamageTime = 0.5f * 60;
	//----------------------------------
};

//ボスの能力データ
struct BossAbility {
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

//ボスの情報取得--------------------------------
BoundingSphere GetBossBS();
Boss_ST* GetBossST();
BossAbility* GetBossABI();
DirectX::XMFLOAT3 GetBossPosition();
DirectX::XMFLOAT3 GetBossRotation();
DirectX::XMFLOAT3 GetBossMove();
DirectX::XMFLOAT3 GetBossScale();
DirectX::SimpleMath::Matrix GetBossMatrix();
//----------------------------------------------

//初期化
void InitBoss();
//ステータス初期化
void InitBoss_ST();
//更新
void UpdateBoss();
//ダメージ計算・ダメージ間隔など----------------
void BossBattle();
void BossDamageHitCalc();
void BossULTDamageHitCalc();
//----------------------------------------------
//描画
void DrawBoss();
//メモリ解放
void ExitBoss();