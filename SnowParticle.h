//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
//
//=============================================================================
#pragma once

#include	<DirectXMath.h>

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
bool InitSnowParticle(void);
void UninitSnowParticle(void);
void UpdateSnowParticle(void);
void DrawSnowParticle(void);

int SetSnowParticle(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 move, DirectX::XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
