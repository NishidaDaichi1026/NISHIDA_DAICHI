#include	"rpgGame.h"
#include	"sceneChange.h"
#include	"3DModel.h"

// 初期処理
bool InitrpgGame() {

	InitScene();
	Init3DModel();
	return true;
}

// 更新処理
void UpdaterpgGame() {

	UpdateScene();
}

// 描画処理
void DrawrpgGame() {

	DrawScene();

}

// ゲーム終了処理
void FinalizerpgGame() {

	ExitScene();
	Uninit3DModel();
}