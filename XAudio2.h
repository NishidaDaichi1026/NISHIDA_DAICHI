//=============================================================================
//
// �T�E���h���� [XAudio2.h]
//
//=============================================================================

#ifndef _XAUDIO2_H_
#define _XAUDIO2_H_
#include <xaudio2.h>

typedef enum
{
	BGM1,	//BGM1	�^�C�g��
	BGM2,	//BGM2	�퓬
	BGM3,	//BGM3	�{�X
	BGM4,	//BGM4	�Q�[���N���A
	BGM5,	//BGM5	�Q�[���I�[�o�[
	SE1,	//SE1	�v���C���[�̍U��
	SE2,	//SE2	����
	SE3,	//SE3	���̃X�L����SE
	SE4,	//SE4	�X�̃X�L����SE
	SE5,	//SE5	�G�̔�eSE�P
	SE6,	//SE6	�G�̔�eSE�Q
	SE7,	//SE7	��eSE
	SE8,	//SE8	�X�L������
	SE9,	//SE9	�I��
	SE10,   //SE10  �H�ׂ�
	SOUND_LABEL_MAX,
} SOUND_LABEL;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSound(void);
void UninitSound(void);
void PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void PauseSound(SOUND_LABEL label);
#endif
