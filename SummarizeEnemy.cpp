#include	"SummarizeEnemy.h"
#include	"Slime.h"
#include	"Magma_Rock.h"

//�G���܂Ƃ߂�t�@�C��
Enemies enemies;

Enemies* GetEnemies(){
	return &enemies;
}
//������
void InitEnemies() {
	InitSlime();
	InitMagma();
	//�G�̍ő吔�擾
	enemies.Enemies_Max = MagmaMax() + SlimeMax();
}
//�X�e�[�^�X������
void InitEnemiesState() {
	InitSlimeState();
	InitMagmaState();
}
//�X�V
void UpdateEnemies(){
	UpdateSlime();
	UpdateMagma();
}
//�`��
void DrawEnemies() {
	DrawSlime();
	DrawMagma();
}
//���������
void ExitEnemies() {
	ExitSlime();
	ExitMagma();
}