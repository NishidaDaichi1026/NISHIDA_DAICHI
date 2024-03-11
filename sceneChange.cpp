#include	<functional>
#include	"sceneChange.h"
#include	"titlescene.h"
#include	"BattleScene.h"
#include	"resultscene.h"
#include	"DemoScene.h"
#include	"BossScene.h"

// カレントシーンインデックス
int g_CurrentSceneIdx = 0;

// 現在シーン
SceneFunctioTable	g_SceneTable[5] = 
{
	// 0 タイトルシーン
	{
		InitTitleScene,
		UpdateTitleScene,
		DrawTitleScene,
		ExitTitleScene,
	},
	// 1 ゲームシーン
	{
		InitBattleScene,
		UpdateBattleScene,
		DrawBattleScene,
		ExitBattleScene,
	},
	// 2 リザルトシーン
	{
		InitResultScene,
		UpdateResultScene,
		DrawResultScene,
		ExitResultScene
	},
	// 3 操作確認シーン
	{
		InitDemoScene,
		UpdateDemoScene,
		DrawDemoScene,
		ExitDemoScene
	},
	// 4 ボスシーン
	{
		InitBossScene,
		UpdateBossScene,
		DrawBossScene,
		ExitBossScene
	}
};

// シーン初期処理
void InitScene() {

	g_SceneTable[g_CurrentSceneIdx].Init();

}

// シーン更新処理
void UpdateScene() {

	g_SceneTable[g_CurrentSceneIdx].Update();

}

// シーン描画処理
void DrawScene() {

	g_SceneTable[g_CurrentSceneIdx].Draw();

}

// シーン終了処理
void ExitScene() {

	g_SceneTable[g_CurrentSceneIdx].Exit();

}

// シーン番号セット
void SetScene(int idx) {
	if (g_CurrentSceneIdx != idx) {
		g_SceneTable[g_CurrentSceneIdx].Exit();
		g_CurrentSceneIdx = idx;
		g_SceneTable[g_CurrentSceneIdx].Init();
	}
}
//	シーンの番号を返す
int ReturnSceneNum() {
	return g_CurrentSceneIdx;
}