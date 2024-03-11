#pragma once

#include	<directxmath.h>

struct BattleScene{

	int SceneChangeCnt = 0;		//敵を倒すと１づつ増える　敵の最大数以上になるとシーン遷移

};

//変数を持っていけるように
BattleScene* GetBattleScene();
// ゲームメインシーン初期化処理
bool InitBattleScene();
// ゲームメインシーン更新処理
void UpdateBattleScene();
// ゲームメインシーン描画処理
void DrawBattleScene();
// ゲームメインシーン終了処理
void ExitBattleScene();