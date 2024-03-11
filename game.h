//*****************************************************************************
//!	@file	game.h
//!	@brief	ゲーム処理
//!	@note	
//!	@author
//*****************************************************************************
#pragma once
#include	<Windows.h>

#define		SCREEN_WIDTH	1920
#define		SCREEN_HEIGHT	1080
#define		FULLSCREEN      1

//-----------------------------------------------------------------------------
// プロトタイプ宣言
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
