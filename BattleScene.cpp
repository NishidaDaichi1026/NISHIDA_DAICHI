#include	"sceneChange.h"
#include	"camera.h"
#include	"3DModel.h"
#include	"CDirectInput.h"
#include	"SummarizeUI.h"
#include	"SummarizeEnemy.h"
#include	"Pause.h"
#include	"BattleScene.h"
#include	"Player.h"
#include	"XAudio2.h"


//-----------------------------------------------------
//				最初のバトルシーンに使用
//-----------------------------------------------------

BattleScene BScene;

static DirectX::SimpleMath::Vector3	set = { 0.0f,0.0f,0.0f };	//シーン切り替え時にプレイヤー情報を初期化するときに使用

//情報取得
BattleScene* GetBattleScene() {
	return &BScene;
}

// ゲームメインシーン初期化
bool InitBattleScene() {
	//マウスカーソルを中央に
	CDirectInput::GetInstance().SetCursorPosition();
	//UI初期化	
	InitSummarizeUI();
	//BGM
	PlaySound(BGM2);
	//呼び出されたらtrueで返す
	return true;
}

// ゲームメインシーン更新処理
void UpdateBattleScene(){

	Enemies* enemies = GetEnemies();
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
			SetPlayerPosition(set);		//プレイヤーの位置を初期化
			SetPlayerRotation(set);		//プレイヤーの回転を初期化
			BScene.SceneChangeCnt = 0;	//繰返しプレイできるように初期化
			StopSound(BGM2);
			pause->PauseDraw = 0;
			SetScene(0);				//ボスシーンに遷移
			break;
			//ゲーム画面に
		case 1:
			pause->PauseDraw = 0;
			break;
		default:
			break;
		}
	}

	//カウントが敵の最大数以上になればシーン遷移
	if (BScene.SceneChangeCnt >= enemies->Enemies_Max) {
		SetPlayerPosition(set);		//プレイヤーの位置を初期化
		SetPlayerRotation(set);		//プレイヤーの回転を初期化
		BScene.SceneChangeCnt = 0;	//繰返しプレイできるように初期化
		StopSound(BGM2);			
		SetScene(4);				//ボスシーンに遷移
	}

	//UI更新
	UpdateSummarizeUI();
}

// ゲームメインシーン描画処理
void DrawBattleScene() {
	
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

// ゲームメインシーン終了処理
void ExitBattleScene() {
	ExitSummarizeUI();
}