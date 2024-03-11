#pragma once

struct Enemies{
	int Enemies_Max = 0;	//敵の最大数取得
};

Enemies* GetEnemies();
//プロトタイプ宣言
void InitEnemies();
void InitEnemiesState();
void UpdateEnemies();
void DrawEnemies();
void ExitEnemies();