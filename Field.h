#pragma once

#include	<SimpleMath.h>
#include	"3DModel.h"
#include	"CPlaneMesh.h"
#include	"CMeshRenderer.h"
#include	"CUndulationPlaneMesh.h"

struct Field
{
	const float FieldX_MAX = 1024.0f;		//フィールドの大きさ
	const float FieldZ_MAX = 1024.0f;		//フィールドの大きさ
};

void makepalenequatation();
//高さ取得
float  GetFieldHeight(DirectX::SimpleMath::Vector3 pos);
//初期化
void InitField();
//描画
void DrawField();