#include	"sceneChange.h"
#include	"camera.h"
#include	"3DModel.h"
#include	"CDirectInput.h"
#include	"SummarizeUI.h"
#include	"XAudio2.h"
#include	"BossEnemy.h"
#include	"Player.h"
#include	"resultscene.h"
#include	"Pause.h"

// ゲームメインシーン初期化
bool InitBossScene() {
	//マウスカーソルを初期化
	CDirectInput::GetInstance().SetCursorPosition();
	//UI初期化
	InitSummarizeUI();
	//BGM
	PlaySound(BGM3);

	return true;
}

// ゲームメインシーン更新処理
void UpdateBossScene() {

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
			StopSound(BGM3);
			SetScene(0);				//タイトルシーンに遷移
			break;
			//ゲーム画面に
		case 1:
			pause->PauseDraw = 0;
			break;
		default:
			break;
		}
	}

	// ボス撃破で画面遷移（リザルトシーンへ）
	BossAbility* b_Abi = GetBossABI();
	ResultScene* r_Scene = GetResultScene();
	if (b_Abi->HP <= 0) {
		StopSound(BGM3);
		pause->PauseDraw = 0;
		r_Scene->CleorChack = 0;		//リザルトシーンクリア
		SetScene(2);
	}

	//プレイヤーが倒されると画面遷移（リザルトシーンへ）
	PlayerAbility* p_Abi = GetPlayerAbi();
	if (p_Abi->HP <= 0) {
		StopSound(BGM3);
		pause->PauseDraw = 0;
		r_Scene->CleorChack = 1;		//リザルトシーンゲームオーバー
		SetScene(2);
	}
	//UI更新
	UpdateSummarizeUI();
}

// ゲームメインシーン描画処理
void DrawBossScene() {

	// 3dモデル描画
	Draw3DModel();
	//UI描画
	DrawSummarizeUI();
	//ポーズ画面描画
	Pause* pause = GetPauseImg();
	if (pause->PauseDraw == 1) {
		DrawPauseImgBack();
		DrawPauseImgSelect();
		DrawPauseImgText();
	}
}

// ゲームメインシーン終了処理
void ExitBossScene() {
	ExitSummarizeUI();
}