#pragma once
#include	<DirectXMath.h>
#include	"3DModel.h"
#include	"CMeshRenderer.h"
#include	"CBoundingSphere.h"
#include	"CSphereMesh.h"

struct BossWeapon_ST {
	int WeaponHitCheck = 0;	//当たり判定の有効、無効を決める　0=無効　　1=有効	
	int attcheck = 0;		//攻撃のアニメーションをするか	  0=しない　1=する
	float AttAnimeTime = 0.7f * 60;	//攻撃のアニメーション時間(秒＊フレーム)
	float attTime = 0;		//攻撃のアニメーションに使用するのでfloat

	int BossAIChack = 0;	//ボスの攻撃間隔のカウントを取る
	const int BossAttInterval = 3 * 60;		//ボスの攻撃間隔を決める(秒＊フレーム)
};

//ボスの武器の情報取得-------------------------------
BoundingSphere GetBossWeaponBS();
BossWeapon_ST* GetBossWeaponST();
DirectX::SimpleMath::Vector3 GetBWPosition();
DirectX::SimpleMath::Vector3 GetBWScale();
DirectX::SimpleMath::Vector3 GetBWMove();
DirectX::SimpleMath::Vector3 GetBWRotation();
DirectX::SimpleMath::Matrix GetBWMatrix();
//---------------------------------------------------
//初期化
void InitBossWeapon();
//更新
void UpdateBossWeapon();
//ダメージ計算
void BossDamageCalc();
//描画
void DrawBossWeapon();
//メモリ解放
void ExitBossWeapon();
