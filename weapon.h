#pragma once

#include	<DirectXMath.h>
#include	"3DModel.h"
#include	"CMeshRenderer.h"
#include	"CBoundingSphere.h"
#include	"CSphereMesh.h"

struct Weapon_ST{
	int WeaponHitCheck = 0;				//当たり判定有り＝１無し＝０
	float attTime = 0.0f;				//攻撃のアニメーションに使用
	float attackCoolTime = 0.5f * 60;	//攻撃のクールタイム　秒数＊フレーム
	//------------------------------------------------------------------------------
	float chargeTime = 1.0f * 60;		//チャージ攻撃可能時間
	float charge = 0.0f;				//チャージ時間
	//外部に持っていく
	float chargeMag = 1.8f;				//チャージ攻撃倍率
	float chargeAtMove = 2.0f;			//チャージ攻撃の移動倍率
	bool Max_charge = false;			//溜まっているか
	//------------------------------------------------------------------------------
};

//ヒットストップ
struct HitStop {
	bool hitchack = false;			//敵に当たるとtrueで返す
	float hitstopTime = 0.1f * 60;	//止めたい秒数＊フレーム
	float hitstopCnt = 0.0f;		//秒をカウントする変数
};

//情報取得-----------------------------------
BoundingSphere GetWeaponBS();
Weapon_ST* GetWeaponST();
HitStop* GetHitStop();
DirectX::XMFLOAT3 GetPositionWeapon();
//プロトタイプ宣言---------------------------
void InitWeapon();			//初期化
void ExitWeapon();			//メモリ解放
void UpdateWeapon();		//更新
void DrawWeapon();			//描画
//-------------------------------------------