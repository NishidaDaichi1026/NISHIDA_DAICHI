#pragma once

struct Pause {

	//�I��؂�ւ�
	int selectNum = 0;
	//�|�[�Y��ʂ̕\���؂�ւ�
	int PauseDraw = 0;
	//�|�[�Y��ʂ̑I���̉摜�̈ʒu�ς�
	float selectpos = 0.0f;
};

Pause* GetPauseImg();

// �|�[�Y�摜��������
void InitPauseImg();
// �|�[�Y�摜�X�V����
void UpdatePauseImg();
// �|�[�Y�摜�`�揈��

void DrawPauseImgText();
void DrawPauseImgBack();
void DrawPauseImgSelect();

// �|�[�Y�摜�I������
void ExitPauseImg();