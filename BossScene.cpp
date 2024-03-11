#include	"sceneChange.h"
#include	"camera.h"
#include	"3DModel.h"
#include	"CDirectInput.h"
#include	"SummarizeUI.h"
#include	"XAudio2.h"
#include	"BossEnemy.h"
#include	"Player.h"
#include	"resultscene.h"
#include	"Pause.h"

// �Q�[�����C���V�[��������
bool InitBossScene() {
	//�}�E�X�J�[�\����������
	CDirectInput::GetInstance().SetCursorPosition();
	//UI������
	InitSummarizeUI();
	//BGM
	PlaySound(BGM3);

	return true;
}

// �Q�[�����C���V�[���X�V����
void UpdateBossScene() {

	Pause* pause = GetPauseImg();
	//�G�X�P�[�v�Ń|�[�Y��ʕ\��
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_ESCAPE)) {
		pause->PauseDraw = 1;
	}
	//�|�[�Y��ʂ��\������Ă��Ȃ��ƍX�V
	if (pause->PauseDraw == 0) {
		// 3d���f���X�V
		Update3DModel();
	}
	else {
		UpdatePauseImg();
	}
	//�|�[�Y��ʂ��\������Ă��ăG���^�[�������Ə���
	if (CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_RETURN)
		&& pause->PauseDraw == 1) {
		switch (pause->selectNum) {
			//�^�C�g����
		case 0:
			pause->PauseDraw = 0;
			StopSound(BGM3);
			SetScene(0);				//�^�C�g���V�[���ɑJ��
			break;
			//�Q�[����ʂ�
		case 1:
			pause->PauseDraw = 0;
			break;
		default:
			break;
		}
	}

	// �{�X���j�ŉ�ʑJ�ځi���U���g�V�[���ցj
	BossAbility* b_Abi = GetBossABI();
	ResultScene* r_Scene = GetResultScene();
	if (b_Abi->HP <= 0) {
		StopSound(BGM3);
		pause->PauseDraw = 0;
		r_Scene->CleorChack = 0;		//���U���g�V�[���N���A
		SetScene(2);
	}

	//�v���C���[���|�����Ɖ�ʑJ�ځi���U���g�V�[���ցj
	PlayerAbility* p_Abi = GetPlayerAbi();
	if (p_Abi->HP <= 0) {
		StopSound(BGM3);
		pause->PauseDraw = 0;
		r_Scene->CleorChack = 1;		//���U���g�V�[���Q�[���I�[�o�[
		SetScene(2);
	}
	//UI�X�V
	UpdateSummarizeUI();
}

// �Q�[�����C���V�[���`�揈��
void DrawBossScene() {

	// 3d���f���`��
	Draw3DModel();
	//UI�`��
	DrawSummarizeUI();
	//�|�[�Y��ʕ`��
	Pause* pause = GetPauseImg();
	if (pause->PauseDraw == 1) {
		DrawPauseImgBack();
		DrawPauseImgSelect();
		DrawPauseImgText();
	}
}

// �Q�[�����C���V�[���I������
void ExitBossScene() {
	ExitSummarizeUI();
}