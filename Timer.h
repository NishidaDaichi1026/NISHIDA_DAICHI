#pragma once

struct Timer{
	int timer = 0;
};

//������
void InitTimer();
//�^�C��������
void InitTimerCnt();
//�^�C�}�[�J�E���g
void UpdateTimer();
//�`��--------------------------
void DrawTimerDecimal();
void DrawTimerSec();
void DrawTimerMin();
//------------------------------

//�N���A�^�C���`��--------------
void DrawClearTimeSec();
void DrawClearTimeMin();
//------------------------------

//���������
void ExitTimer();
//�^�C�}�[�̕ϐ��擾
Timer* GetTimer();