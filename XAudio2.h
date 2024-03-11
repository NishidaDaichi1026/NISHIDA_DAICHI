//=============================================================================
//
// サウンド処理 [XAudio2.h]
//
//=============================================================================

#ifndef _XAUDIO2_H_
#define _XAUDIO2_H_
#include <xaudio2.h>

typedef enum
{
	BGM1,	//BGM1	タイトル
	BGM2,	//BGM2	戦闘
	BGM3,	//BGM3	ボス
	BGM4,	//BGM4	ゲームクリア
	BGM5,	//BGM5	ゲームオーバー
	SE1,	//SE1	プレイヤーの攻撃
	SE2,	//SE2	決定
	SE3,	//SE3	雷のスキルのSE
	SE4,	//SE4	氷のスキルのSE
	SE5,	//SE5	敵の被弾SE１
	SE6,	//SE6	敵の被弾SE２
	SE7,	//SE7	被弾SE
	SE8,	//SE8	スキル発動
	SE9,	//SE9	選択
	SE10,   //SE10  食べる
	SOUND_LABEL_MAX,
} SOUND_LABEL;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSound(void);
void UninitSound(void);
void PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void PauseSound(SOUND_LABEL label);
#endif
