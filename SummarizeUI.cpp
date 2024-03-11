#include	"AttCom.h"
#include	"AttTex.h"
#include	"TitleCom.h"
#include	"UI.h"
#include	"Timer.h"
#include	"Clock.h"
#include	"SceneFade.h"
#include	"StatusBar.h"
#include	"DamageDis.h"
#include	"Pause.h"

//----------------------------------
//			UI系をまとめる	
//----------------------------------

void InitSummarizeUI() {
	InitAttCom();
	InitAttTex();
	InitUI();
	InitTimer();
	InitClock();
	InitSceneFade();
	InitStatusBar();
	InitDamageDis();
	InitPauseImg();
}

void UpdateSummarizeUI() {
	//ポーズ画面でタイマー停止
	Pause* pause = GetPauseImg();
	if (pause->PauseDraw == 0) {
		UpdateTimer();
	}
}

void DrawSummarizeUI() {
	//ステータスバー----------
	DrawHpBar();
	DrawSkillBar();
	DrawChargeBar();
	//------------------------
	//バトル用UI--------------
	DrawUI();
	DrawDamageDis();
	DrawDamageImg();

	DrawSelCom();
	DrawAttCom();
	DrawAttTex();
	DrawAttackCom();
	DrawSpecialCom();
	
	DrawAttTex_F();
	DrawAttTex_N();
	DrawAttTex_W();
	DrawAttTex_I();
	DrawAttTex_T();
	//------------------------
	//時間--------------------
	DrawClock();
	DrawClockHands();

	DrawTimerDecimal();
	DrawTimerSec();
	DrawTimerMin();
	//------------------------
	//フェード----------------
	DrawBattleSceneFade();
	UpdateBossFadeVal();
	//------------------------
}

void ExitSummarizeUI() {
	ExitAttCom();
	ExitAttTex();
	ExitUI();
	ExitTimer();
	ExitClock();
	ExitSceneFade();
	ExitStatusBar();
	ExitDamageDis();
	ExitPauseImg();
}