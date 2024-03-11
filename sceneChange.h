#pragma once
#include	<functional>

// �V�[�����񋓃N���X
enum class GAMESCENE {
	TITLE = 0,
	FIELD = 1,
	RESULT = 2,
	DEMOPLAY = 3,
	BOSS = 4
};

// �V�[���e�[�u��
struct SceneFunctioTable {
	std::function<bool(void)> Init;		// ��������
	std::function<void(void)> Update;	// �X�V����
	std::function<void(void)> Draw;		// �`�揈��
	std::function<void(void)> Exit;		// �I������
};

// �V�[����������
void InitScene();
// �V�[���X�V����
void UpdateScene();
// �V�[���`�揈��
void DrawScene();
// �V�[���I������
void ExitScene();
// �V�[���Z�b�g
void SetScene(int idx);
// �V�[�����̃��f���̍X�V�����ύX
int ReturnSceneNum();