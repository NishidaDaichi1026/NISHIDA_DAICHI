#pragma once

#include	<directxmath.h>

struct ResultScene {

	int CleorChack = 0;		//クリア（０）かゲームオーバー（１）か

};

// リザルトシーン初期処理
bool InitResultScene();
// リザルトシーン更新処理
void UpdateResultScene();
// リザルトシーン描画処理
void DrawResultScene();
// リザルトシーン終了処理
void ExitResultScene();

ResultScene* GetResultScene();