#pragma once

struct Pause {

	//選択切り替え
	int selectNum = 0;
	//ポーズ画面の表示切り替え
	int PauseDraw = 0;
	//ポーズ画面の選択の画像の位置変え
	float selectpos = 0.0f;
};

Pause* GetPauseImg();

// ポーズ画像初期処理
void InitPauseImg();
// ポーズ画像更新処理
void UpdatePauseImg();
// ポーズ画像描画処理

void DrawPauseImgText();
void DrawPauseImgBack();
void DrawPauseImgSelect();

// ポーズ画像終了処理
void ExitPauseImg();