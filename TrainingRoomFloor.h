#pragma once

#include	<SimpleMath.h>
#include	"3DModel.h"
#include	"CPlaneMesh.h"
#include	"CMeshRenderer.h"
#include	"CUndulationPlaneMesh.h"

struct TRoom
{
	const float TRoomX_MAX = 500.0f;		//フィールドの大きさ
	const float TRoomZ_MAX = 500.0f;		//フィールドの大きさ
};
//高さ取得
float  GetTRoomHeight(DirectX::SimpleMath::Vector3 pos);
//初期化
void InitTRoom();
//描画
void DrawTRoom();