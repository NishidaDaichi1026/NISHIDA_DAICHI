#pragma once
#include	<functional>

// シーン名列挙クラス
enum class GAMESCENE {
	TITLE = 0,
	FIELD = 1,
	RESULT = 2,
	DEMOPLAY = 3,
	BOSS = 4
};

// シーンテーブル
struct SceneFunctioTable {
	std::function<bool(void)> Init;		// 初期処理
	std::function<void(void)> Update;	// 更新処理
	std::function<void(void)> Draw;		// 描画処理
	std::function<void(void)> Exit;		// 終了処理
};

// シーン初期処理
void InitScene();
// シーン更新処理
void UpdateScene();
// シーン描画処理
void DrawScene();
// シーン終了処理
void ExitScene();
// シーンセット
void SetScene(int idx);
// シーン毎のモデルの更新処理変更
int ReturnSceneNum();