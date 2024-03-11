#pragma once

#include	<SimpleMath.h>
#include	"3DModel.h"
#include	"CPlaneMesh.h"
#include	"CMeshRenderer.h"
#include	"CUndulationPlaneMesh.h"

struct TRoom
{
	const float TRoomX_MAX = 500.0f;		//�t�B�[���h�̑傫��
	const float TRoomZ_MAX = 500.0f;		//�t�B�[���h�̑傫��
};
//�����擾
float  GetTRoomHeight(DirectX::SimpleMath::Vector3 pos);
//������
void InitTRoom();
//�`��
void DrawTRoom();