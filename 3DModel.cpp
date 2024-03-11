#include	"3DModel.h"
#include	"SummarizeEnemy.h"
#include	"Player.h"
#include	"BossEnemy.h"
#include	"weapon.h"
#include	"sceneChange.h"
#include	"SceneFade.h"
#include	"Field.h"
#include	"sky.h"
#include	"Wall.h"
#include	"Kakashi.h"
#include	"TrainingRoomFloor.h"
#include	"TrainingRoomWall.h"

//---------------------------------------------------------------
//					３Dモデルをまとめる
//---------------------------------------------------------------

Camera g_camera;
Fade g_fade;

//初期化
void Init3DModel() {

	// プレイヤ初期化
	g_camera.Init();
	InitPlayer();
	InitBoss();
	InitEnemies();
	InitWeapon();
	InitKakashi();

	// フィールド初期化
	InitField();
	InitTRoom();
	InitTR_Wall();
	InitSky();
	InitWall();

}
//メモリ解放
void Uninit3DModel() {

	ExitPlayer();
	ExitEnemies();
	ExitBoss();
	ExitWeapon();
	ExitKakashi();

}
//更新
void Update3DModel() {
	int updateEnemy = ReturnSceneNum();		//1=バトルシーン　4=ボスシーン
	Fade* fade = GetFade();					//フェードのカウント変数を持ってくる
	HitStop* h_stop = GetHitStop();			//ヒットストップの変数を持ってくる
	//攻撃ヒットで〇秒間更新しない（ヒットストップ）
	if (h_stop->hitchack) {
		h_stop->hitstopCnt += 1.0f;
		if (h_stop->hitstopCnt >= h_stop->hitstopTime) {
			h_stop->hitchack = false;
			h_stop->hitstopCnt = 0.0f;
		}
	}
	else {
		//バトルシーンの更新（フェード終了時から始める）
		if (updateEnemy == 1 && g_fade.BS_FadeOutTime >= (g_fade.BS_Fadecnt * 60)) {	//整数＊フレーム数（６０ｆ）
			UpdatePlayer();
			UpdateEnemies();
			UpdateWeapon();
			g_camera.UpdateBS();
		}
		//ボスシーンの更新
		if (updateEnemy == 4) {
			g_camera.UpdateBossS();
			if (BossFadeCount() >= fade->BossDirEnd) {		//演出が終わってから動き出す
				UpdatePlayer();
				UpdateWeapon();
				UpdateBoss();
			}
		}
		//デモプレイシーンの更新
		if (updateEnemy == 3) {
			UpdatePlayer();
			UpdateKakashi();
			UpdateWeapon();
			g_camera.UpdateBS();
		}
	}
}
//描画
void Draw3DModel() {
	int drawEnemy = ReturnSceneNum();		//1=バトルシーン　4=ボスシーン
	g_camera.Draw();

	//バトルシーンの描画
	if (drawEnemy == 1) {
		DrawEnemies();
		DrawSky();
		// フィールド描画
		DrawField();
	}
	//ボスシーンの描画
	if (drawEnemy == 4) {
		DrawBoss();
		DrawWall();
		// フィールド描画
		DrawField();
	}
	if (drawEnemy == 3) {
		DrawKakashi();
		DrawSky();
		DrawTRoom();
		DrawTR_Wall();
	}

	// プレイヤ描画
	DrawPlayer();
	DrawWeapon();
}