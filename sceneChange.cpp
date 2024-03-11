#include	<functional>
#include	"sceneChange.h"
#include	"titlescene.h"
#include	"BattleScene.h"
#include	"resultscene.h"
#include	"DemoScene.h"
#include	"BossScene.h"

// �J�����g�V�[���C���f�b�N�X
int g_CurrentSceneIdx = 0;

// ���݃V�[��
SceneFunctioTable	g_SceneTable[5] = 
{
	// 0 �^�C�g���V�[��
	{
		InitTitleScene,
		UpdateTitleScene,
		DrawTitleScene,
		ExitTitleScene,
	},
	// 1 �Q�[���V�[��
	{
		InitBattleScene,
		UpdateBattleScene,
		DrawBattleScene,
		ExitBattleScene,
	},
	// 2 ���U���g�V�[��
	{
		InitResultScene,
		UpdateResultScene,
		DrawResultScene,
		ExitResultScene
	},
	// 3 ����m�F�V�[��
	{
		InitDemoScene,
		UpdateDemoScene,
		DrawDemoScene,
		ExitDemoScene
	},
	// 4 �{�X�V�[��
	{
		InitBossScene,
		UpdateBossScene,
		DrawBossScene,
		ExitBossScene
	}
};

// �V�[����������
void InitScene() {

	g_SceneTable[g_CurrentSceneIdx].Init();

}

// �V�[���X�V����
void UpdateScene() {

	g_SceneTable[g_CurrentSceneIdx].Update();

}

// �V�[���`�揈��
void DrawScene() {

	g_SceneTable[g_CurrentSceneIdx].Draw();

}

// �V�[���I������
void ExitScene() {

	g_SceneTable[g_CurrentSceneIdx].Exit();

}

// �V�[���ԍ��Z�b�g
void SetScene(int idx) {
	if (g_CurrentSceneIdx != idx) {
		g_SceneTable[g_CurrentSceneIdx].Exit();
		g_CurrentSceneIdx = idx;
		g_SceneTable[g_CurrentSceneIdx].Init();
	}
}
//	�V�[���̔ԍ���Ԃ�
int ReturnSceneNum() {
	return g_CurrentSceneIdx;
}