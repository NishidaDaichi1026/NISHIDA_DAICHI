#include	"sceneChange.h"
#include	"camera.h"
#include	"3DModel.h"
#include	"CDirectInput.h"
#include	"SummarizeUI.h"
#include	"SummarizeEnemy.h"
#include	"Pause.h"
#include	"BattleScene.h"
#include	"Player.h"
#include	"XAudio2.h"


//-----------------------------------------------------
//				�ŏ��̃o�g���V�[���Ɏg�p
//-----------------------------------------------------

BattleScene BScene;

static DirectX::SimpleMath::Vector3	set = { 0.0f,0.0f,0.0f };	//�V�[���؂�ւ����Ƀv���C���[��������������Ƃ��Ɏg�p

//���擾
BattleScene* GetBattleScene() {
	return &BScene;
}

// �Q�[�����C���V�[��������
bool InitBattleScene() {
	//�}�E�X�J�[�\���𒆉���
	CDirectInput::GetInstance().SetCursorPosition();
	//UI������	
	InitSummarizeUI();
	//BGM
	PlaySound(BGM2);
	//�Ăяo���ꂽ��true�ŕԂ�
	return true;
}

// �Q�[�����C���V�[���X�V����
void UpdateBattleScene(){

	Enemies* enemies = GetEnemies();
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
			SetPlayerPosition(set);		//�v���C���[�̈ʒu��������
			SetPlayerRotation(set);		//�v���C���[�̉�]��������
			BScene.SceneChangeCnt = 0;	//�J�Ԃ��v���C�ł���悤�ɏ�����
			StopSound(BGM2);
			pause->PauseDraw = 0;
			SetScene(0);				//�{�X�V�[���ɑJ��
			break;
			//�Q�[����ʂ�
		case 1:
			pause->PauseDraw = 0;
			break;
		default:
			break;
		}
	}

	//�J�E���g���G�̍ő吔�ȏ�ɂȂ�΃V�[���J��
	if (BScene.SceneChangeCnt >= enemies->Enemies_Max) {
		SetPlayerPosition(set);		//�v���C���[�̈ʒu��������
		SetPlayerRotation(set);		//�v���C���[�̉�]��������
		BScene.SceneChangeCnt = 0;	//�J�Ԃ��v���C�ł���悤�ɏ�����
		StopSound(BGM2);			
		SetScene(4);				//�{�X�V�[���ɑJ��
	}

	//UI�X�V
	UpdateSummarizeUI();
}

// �Q�[�����C���V�[���`�揈��
void DrawBattleScene() {
	
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

// �Q�[�����C���V�[���I������
void ExitBattleScene() {
	ExitSummarizeUI();
}