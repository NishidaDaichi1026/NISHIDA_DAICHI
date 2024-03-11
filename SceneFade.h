#pragma once

struct Fade {
	//バトルシーン開始時のフェードに使用
	int BS_Fadecnt = 0;
	int BS_FadeOutTime = 1;	//フェードイン開始時間（秒で記入）

	//ボスシーン開始時の演出に使用
	float BossFadecnt = 0.0f;
	//以下各演出の開始時間取得に使用----------
	// ○○ = 時間　* フレーム
	//時間を変更したら開始時間が変わる
	float BossDir1 = 0.5f * 60;
	float BossDir2 = 2 * 60;
	float BossDir3 = 4 * 60;
	float BossDirEnd = 7.5f * 60;
	//----------------------------------------
	//ボスシーンの演出（右、左への移動）
	float BossEncImgR = 0.0f;
	float BossEncImgL = 0.0f;
	//フェード用画像の透過率
	float BattleSceneFade = 1.0f;
	float BossFade = 1.0f;
	float BossDirectionFade = 0.0f;
};
//変数取得
Fade* GetFade();
//初期化
void InitSceneFade();
//変数の初期化
void InitFadeVal();
//バトルシーン開始時のフェード
void DrawBattleSceneFade();
//ボスシーンに使用-----------------------
void UpdateBossFadeVal();
void DrawBossFade();
void DrawBossDirection1();
void DrawBossDirection2();
void DrawBossDirection3();
//---------------------------------------
//メモリ解放
void ExitSceneFade();
//演出時間取得（終わるまでボスとプレイヤーを更新しない）
int BossFadeCount();