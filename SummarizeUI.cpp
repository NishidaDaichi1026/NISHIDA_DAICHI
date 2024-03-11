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
//			UI�n���܂Ƃ߂�	
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
	//�|�[�Y��ʂŃ^�C�}�[��~
	Pause* pause = GetPauseImg();
	if (pause->PauseDraw == 0) {
		UpdateTimer();
	}
}

void DrawSummarizeUI() {
	//�X�e�[�^�X�o�[----------
	DrawHpBar();
	DrawSkillBar();
	DrawChargeBar();
	//------------------------
	//�o�g���pUI--------------
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
	//����--------------------
	DrawClock();
	DrawClockHands();

	DrawTimerDecimal();
	DrawTimerSec();
	DrawTimerMin();
	//------------------------
	//�t�F�[�h----------------
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