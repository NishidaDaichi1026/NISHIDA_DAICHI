#include	"rpgGame.h"
#include	"sceneChange.h"
#include	"3DModel.h"

// ��������
bool InitrpgGame() {

	InitScene();
	Init3DModel();
	return true;
}

// �X�V����
void UpdaterpgGame() {

	UpdateScene();
}

// �`�揈��
void DrawrpgGame() {

	DrawScene();

}

// �Q�[���I������
void FinalizerpgGame() {

	ExitScene();
	Uninit3DModel();
}