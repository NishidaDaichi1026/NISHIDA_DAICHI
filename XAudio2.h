//=============================================================================
//
// TEh [XAudio2.h]
//
//=============================================================================

#ifndef _XAUDIO2_H_
#define _XAUDIO2_H_
#include <xaudio2.h>

typedef enum
{
	BGM1,	//BGM1	^Cg
	BGM2,	//BGM2	í¬
	BGM3,	//BGM3	{X
	BGM4,	//BGM4	Q[NA
	BGM5,	//BGM5	Q[I[o[
	SE1,	//SE1	vC[ÌU
	SE2,	//SE2	è
	SE3,	//SE3	ÌXLÌSE
	SE4,	//SE4	XÌXLÌSE
	SE5,	//SE5	GÌíeSEP
	SE6,	//SE6	GÌíeSEQ
	SE7,	//SE7	íeSE
	SE8,	//SE8	XL­®
	SE9,	//SE9	Ið
	SE10,   //SE10  H×é
	SOUND_LABEL_MAX,
} SOUND_LABEL;

//*****************************************************************************
// vg^Cvé¾
//*****************************************************************************
HRESULT InitSound(void);
void UninitSound(void);
void PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void PauseSound(SOUND_LABEL label);
#endif
