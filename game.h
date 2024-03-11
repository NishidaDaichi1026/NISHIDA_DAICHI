//*****************************************************************************
//!	@file	game.h
//!	@brief	�Q�[������
//!	@note	
//!	@author
//*****************************************************************************
#pragma once
#include	<Windows.h>

#define		SCREEN_WIDTH	1920
#define		SCREEN_HEIGHT	1080
#define		FULLSCREEN      1

//-----------------------------------------------------------------------------
// �v���g�^�C�v�錾
//-----------------------------------------------------------------------------
bool GameInit(HINSTANCE hinst, HWND hwnd, int width, int height, bool fullscreen);
void GameMain();
void GameExit();
void GameInput();
void GameUpdate();
void GameRender();

//******************************************************************************
//	End of file.
//******************************************************************************
