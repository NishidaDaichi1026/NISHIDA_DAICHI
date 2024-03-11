#pragma once

#include	<SimpleMath.h>
#include	"3DModel.h"
#include	"CPlaneMesh.h"
#include	"CMeshRenderer.h"
#include	"CUndulationPlaneMesh.h"

struct Field
{
	const float FieldX_MAX = 1024.0f;		//�t�B�[���h�̑傫��
	const float FieldZ_MAX = 1024.0f;		//�t�B�[���h�̑傫��
};

void makepalenequatation();
//�����擾
float  GetFieldHeight(DirectX::SimpleMath::Vector3 pos);
//������
void InitField();
//�`��
void DrawField();