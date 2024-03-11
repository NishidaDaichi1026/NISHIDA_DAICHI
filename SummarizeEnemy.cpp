#include	"SummarizeEnemy.h"
#include	"Slime.h"
#include	"Magma_Rock.h"

//敵をまとめるファイル
Enemies enemies;

Enemies* GetEnemies(){
	return &enemies;
}
//初期化
void InitEnemies() {
	InitSlime();
	InitMagma();
	//敵の最大数取得
	enemies.Enemies_Max = MagmaMax() + SlimeMax();
}
//ステータス初期化
void InitEnemiesState() {
	InitSlimeState();
	InitMagmaState();
}
//更新
void UpdateEnemies(){
	UpdateSlime();
	UpdateMagma();
}
//描画
void DrawEnemies() {
	DrawSlime();
	DrawMagma();
}
//メモリ解放
void ExitEnemies() {
	ExitSlime();
	ExitMagma();
}