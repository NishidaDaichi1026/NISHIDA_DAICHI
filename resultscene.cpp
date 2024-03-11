#include	"sceneChange.h"
#include	"camera.h"
#include	"CDirectInput.h"
#include	"resultscene.h"
#include	"Timer.h"
#include	"ResultImg.h"
#include	"XAudio2.h"
#include	"Kamifubuki.h"

ResultScene RScene;

ResultScene* GetResultScene() {
	return &RScene;
}

bool InitResultScene() {
	InitResultImg();
	InitTimer();
	InitKamifubuki();
	if (RScene.CleorChack == 0) {		//クリアBGM
		PlaySound(BGM4);
	}
	if (RScene.CleorChack == 1) {		//ゲームオーバーBGM
		PlaySound(BGM5);
	}
	return true;
}

void UpdateResultScene() {

	// リターンキー入力で画面遷移（ゲームシーンへ）
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_RETURN)) {
		if (RScene.CleorChack == 0) {
			StopSound(BGM4);
		}
		if (RScene.CleorChack == 1){
			StopSound(BGM5);
		}
		SetScene(0);
	}
}

void DrawResultScene() {

	if (RScene.CleorChack == 0) {
		DrawResultImgClear();
		//クリアタイム
		DrawClearTimeMin();
		DrawClearTimeSec();

		DrawKamifubuki();
	}
	else{
		DrawResultImgGameOver();
	}
	
}

void ExitResultScene() {
	ExitResultImg();
	ExitTimer();
	ExitKamifubuki();
}
