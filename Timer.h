#pragma once

struct Timer{
	int timer = 0;
};

//初期化
void InitTimer();
//タイム初期化
void InitTimerCnt();
//タイマーカウント
void UpdateTimer();
//描画--------------------------
void DrawTimerDecimal();
void DrawTimerSec();
void DrawTimerMin();
//------------------------------

//クリアタイム描画--------------
void DrawClearTimeSec();
void DrawClearTimeMin();
//------------------------------

//メモリ解放
void ExitTimer();
//タイマーの変数取得
Timer* GetTimer();