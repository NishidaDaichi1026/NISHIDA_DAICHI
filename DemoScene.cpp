#include	"sceneChange.h"
#include	"3DModel.h"
#include	"TrainingRoomFloor.h"
#include	"CDirectInput.h"
#include	"SummarizeUI.h"
#include	"Pause.h"
#include	"Player.h"
#include	"XAudio2.h"

// 初期化
bool InitDemoScene() {

	CDirectInput::GetInstance().SetCursorPosition();

	InitSummarizeUI();
	PlaySound(BGM2);

	return true;
}

// 更新処理
void UpdateDemoScene() {

	Pause* pause = GetPauseImg();

	//エスケープでポーズ画面表示
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_ESCAPE)) {
		pause->PauseDraw = 1;
	}
	//ポーズ画面が表示されていないと更新
	if (pause->PauseDraw == 0) {
		// 3dモデル更新
		Update3DModel();
	}
	else {
		UpdatePauseImg();
	}
	//ポーズ画面が表示されていてエンターを押すと処理
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_RETURN)
		&& pause->PauseDraw == 1) {
		switch (pause->selectNum) {
			//タイトルに
		case 0:
			pause->PauseDraw = 0;
			StopSound(BGM2);
			SetScene(0);
			break;
			//ゲーム画面に
		case 1:
			pause->PauseDraw = 0;
			break;
		default:
			break;
		}
	}

	
}

// 描画処理
void DrawDemoScene() {

	Pause* pause = GetPauseImg();
	// 3dモデル描画
	Draw3DModel();
	//UI描画
	DrawSummarizeUI();
	if (pause->PauseDraw == 1) {
		DrawPauseImgBack();
		DrawPauseImgSelect();
		DrawPauseImgText();
	}
}

// 終了処理
void ExitDemoScene() {
	ExitSummarizeUI();
}