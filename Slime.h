#pragma once
#include	<directxmath.h>
#include	"3DModel.h"
#include	"CPlaneMesh.h"
#include	"CMeshRenderer.h"

// プロトタイプ宣言

void InitSlime();
void InitSlimeState();
void UpdateSlime();
void DrawSlime();
void ExitSlime();
int SlimeMax();

// 位置取得
DirectX::XMFLOAT3 GetPositionSlime(int idx);
// 姿勢取得
DirectX::XMFLOAT3 GetRotationSlime(int idx);
// 移動量取得
DirectX::XMFLOAT3 GetMoveSlime(int idx);