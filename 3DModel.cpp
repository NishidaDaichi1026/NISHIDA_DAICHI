#include	"3DModel.h"
#include	"SummarizeEnemy.h"
#include	"Player.h"
#include	"BossEnemy.h"
#include	"weapon.h"
#include	"sceneChange.h"
#include	"SceneFade.h"
#include	"Field.h"
#include	"sky.h"
#include	"Wall.h"
#include	"Kakashi.h"
#include	"TrainingRoomFloor.h"
#include	"TrainingRoomWall.h"

//---------------------------------------------------------------
//					�RD���f�����܂Ƃ߂�
//---------------------------------------------------------------

Camera g_camera;
Fade g_fade;

//������
void Init3DModel() {

	// �v���C��������
	g_camera.Init();
	InitPlayer();
	InitBoss();
	InitEnemies();
	InitWeapon();
	InitKakashi();

	// �t�B�[���h������
	InitField();
	InitTRoom();
	InitTR_Wall();
	InitSky();
	InitWall();

}
//���������
void Uninit3DModel() {

	ExitPlayer();
	ExitEnemies();
	ExitBoss();
	ExitWeapon();
	ExitKakashi();

}
//�X�V
void Update3DModel() {
	int updateEnemy = ReturnSceneNum();		//1=�o�g���V�[���@4=�{�X�V�[��
	Fade* fade = GetFade();					//�t�F�[�h�̃J�E���g�ϐ��������Ă���
	HitStop* h_stop = GetHitStop();			//�q�b�g�X�g�b�v�̕ϐ��������Ă���
	//�U���q�b�g�ŁZ�b�ԍX�V���Ȃ��i�q�b�g�X�g�b�v�j
	if (h_stop->hitchack) {
		h_stop->hitstopCnt += 1.0f;
		if (h_stop->hitstopCnt >= h_stop->hitstopTime) {
			h_stop->hitchack = false;
			h_stop->hitstopCnt = 0.0f;
		}
	}
	else {
		//�o�g���V�[���̍X�V�i�t�F�[�h�I��������n�߂�j
		if (updateEnemy == 1 && g_fade.BS_FadeOutTime >= (g_fade.BS_Fadecnt * 60)) {	//�������t���[�����i�U�O���j
			UpdatePlayer();
			UpdateEnemies();
			UpdateWeapon();
			g_camera.UpdateBS();
		}
		//�{�X�V�[���̍X�V
		if (updateEnemy == 4) {
			g_camera.UpdateBossS();
			if (BossFadeCount() >= fade->BossDirEnd) {		//���o���I����Ă��瓮���o��
				UpdatePlayer();
				UpdateWeapon();
				UpdateBoss();
			}
		}
		//�f���v���C�V�[���̍X�V
		if (updateEnemy == 3) {
			UpdatePlayer();
			UpdateKakashi();
			UpdateWeapon();
			g_camera.UpdateBS();
		}
	}
}
//�`��
void Draw3DModel() {
	int drawEnemy = ReturnSceneNum();		//1=�o�g���V�[���@4=�{�X�V�[��
	g_camera.Draw();

	//�o�g���V�[���̕`��
	if (drawEnemy == 1) {
		DrawEnemies();
		DrawSky();
		// �t�B�[���h�`��
		DrawField();
	}
	//�{�X�V�[���̕`��
	if (drawEnemy == 4) {
		DrawBoss();
		DrawWall();
		// �t�B�[���h�`��
		DrawField();
	}
	if (drawEnemy == 3) {
		DrawKakashi();
		DrawSky();
		DrawTRoom();
		DrawTR_Wall();
	}

	// �v���C���`��
	DrawPlayer();
	DrawWeapon();
}