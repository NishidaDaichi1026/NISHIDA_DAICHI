#pragma once

#include	<directxmath.h>

struct ResultScene {

	int CleorChack = 0;		//�N���A�i�O�j���Q�[���I�[�o�[�i�P�j��

};

// ���U���g�V�[����������
bool InitResultScene();
// ���U���g�V�[���X�V����
void UpdateResultScene();
// ���U���g�V�[���`�揈��
void DrawResultScene();
// ���U���g�V�[���I������
void ExitResultScene();

ResultScene* GetResultScene();