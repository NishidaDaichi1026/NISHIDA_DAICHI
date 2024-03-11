#pragma once
#include	<directxmath.h>
#include	"3DModel.h"
#include	"CPlaneMesh.h"
#include	"CMeshRenderer.h"

// プロトタイプ宣言
void InitMagma();
void InitMagmaState();
void UpdateMagma();
void DrawMagma();
void ExitMagma();
int MagmaMax();

// 位置取得
DirectX::XMFLOAT3 GetPositionMagma(int idx);
// 姿勢取得
DirectX::XMFLOAT3 GetRotationMagma(int idx);
// 移動量取得
DirectX::XMFLOAT3 GetMoveMagma(int idx);