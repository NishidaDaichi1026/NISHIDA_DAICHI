#pragma once

struct TitleLogo {

	//�^�C�g�����S���㉺������
	int UpDown = 0;
	//�㉺�X�s�[�h
	float Speed = 0.01f;
	//�㉺��
	float SwingW = 20.0f;
};

// UI��������
void InitTitleImg();
// UI�`�揈��
void DrawTitleImg();
//�^�C�g�����S
void DrawTitleLogo();
//�^�C�g���̑������
void DrawInstOp();
// UI�I������
void ExitTitleImg();