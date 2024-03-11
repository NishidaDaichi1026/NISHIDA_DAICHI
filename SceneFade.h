#pragma once

struct Fade {
	//�o�g���V�[���J�n���̃t�F�[�h�Ɏg�p
	int BS_Fadecnt = 0;
	int BS_FadeOutTime = 1;	//�t�F�[�h�C���J�n���ԁi�b�ŋL���j

	//�{�X�V�[���J�n���̉��o�Ɏg�p
	float BossFadecnt = 0.0f;
	//�ȉ��e���o�̊J�n���Ԏ擾�Ɏg�p----------
	// ���� = ���ԁ@* �t���[��
	//���Ԃ�ύX������J�n���Ԃ��ς��
	float BossDir1 = 0.5f * 60;
	float BossDir2 = 2 * 60;
	float BossDir3 = 4 * 60;
	float BossDirEnd = 7.5f * 60;
	//----------------------------------------
	//�{�X�V�[���̉��o�i�E�A���ւ̈ړ��j
	float BossEncImgR = 0.0f;
	float BossEncImgL = 0.0f;
	//�t�F�[�h�p�摜�̓��ߗ�
	float BattleSceneFade = 1.0f;
	float BossFade = 1.0f;
	float BossDirectionFade = 0.0f;
};
//�ϐ��擾
Fade* GetFade();
//������
void InitSceneFade();
//�ϐ��̏�����
void InitFadeVal();
//�o�g���V�[���J�n���̃t�F�[�h
void DrawBattleSceneFade();
//�{�X�V�[���Ɏg�p-----------------------
void UpdateBossFadeVal();
void DrawBossFade();
void DrawBossDirection1();
void DrawBossDirection2();
void DrawBossDirection3();
//---------------------------------------
//���������
void ExitSceneFade();
//���o���Ԏ擾�i�I���܂Ń{�X�ƃv���C���[���X�V���Ȃ��j
int BossFadeCount();