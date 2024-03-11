#pragma once

struct TitleLogo {

	//タイトルロゴを上下させる
	int UpDown = 0;
	//上下スピード
	float Speed = 0.01f;
	//上下幅
	float SwingW = 20.0f;
};

// UI初期処理
void InitTitleImg();
// UI描画処理
void DrawTitleImg();
//タイトルロゴ
void DrawTitleLogo();
//タイトルの操作説明
void DrawInstOp();
// UI終了処理
void ExitTitleImg();