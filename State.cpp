#include	"State.h"

//��{�X�e�[�^�X	�������������Ƃ��͂������玝���Ă���
ST state[] ={
	//ID,���O,HP,ATTACK,DEFENCE,SPEED
	 {0,"�v���C���[", 2000, 500, 100, 1.0f},
	 {1,"�X���C��", 1500, 150, 64, 1.2f},
	 {2,"�}�O�}��", 1000, 100, 250, 0.6f},
	 {3,"�~�m�^�E���X", 5000, 256, 128, 0.8f},
	 {4,"�J�J�V",100000,0,123,0.0f}
};

//�����̔{��
float weak = 2.0f;		//��_
float equal = 1.0f;		//���{
float subtle = 0.5f;	//���܂���
float invalid = 0.0f;	//����

//�����̃X�e�[�^�X (ID, �v���C���[�E�G�̑���, �e�����ɑ΂���ϐ�)
ATTRIBUTE attr[] = {
	//ID,����,���{��,���{��,���{��,�X�{��,���{��
	 {0,"��",equal, equal, equal, equal, equal},
	 {1,"��",equal, invalid, weak, subtle, equal},
	 {2,"��",equal, subtle, invalid, subtle, weak},
	 {3,"�X",equal, weak, subtle, invalid, equal},
	 {4,"��",equal, equal, weak, equal, invalid}
};