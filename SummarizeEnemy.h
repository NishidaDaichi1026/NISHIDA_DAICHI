#pragma once

struct Enemies{
	int Enemies_Max = 0;	//�G�̍ő吔�擾
};

Enemies* GetEnemies();
//�v���g�^�C�v�錾
void InitEnemies();
void InitEnemiesState();
void UpdateEnemies();
void DrawEnemies();
void ExitEnemies();