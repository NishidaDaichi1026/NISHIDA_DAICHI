#include	"sceneChange.h"
#include	"3DModel.h"
#include	"TrainingRoomFloor.h"
#include	"CDirectInput.h"
#include	"SummarizeUI.h"
#include	"Pause.h"
#include	"Player.h"
#include	"XAudio2.h"

// ������
bool InitDemoScene() {

	CDirectInput::GetInstance().SetCursorPosition();

	InitSummarizeUI();
	PlaySound(BGM2);

	return true;
}

// �X�V����
void UpdateDemoScene() {

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
			StopSound(BGM2);
			SetScene(0);
			break;
			//�Q�[����ʂ�
		case 1:
			pause->PauseDraw = 0;
			break;
		default:
			break;
		}
	}

	
}

// �`�揈��
void DrawDemoScene() {

	Pause* pause = GetPauseImg();
	// 3d���f���`��
	Draw3DModel();
	//UI�`��
	DrawSummarizeUI();
	if (pause->PauseDraw == 1) {
		DrawPauseImgBack();
		DrawPauseImgSelect();
		DrawPauseImgText();
	}
}

// �I������
void ExitDemoScene() {
	ExitSummarizeUI();
}