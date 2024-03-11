#pragma once
#include	<directxmath.h>
#include	"3DModel.h"
#include	"CPlaneMesh.h"
#include	"CMeshRenderer.h"

// �v���g�^�C�v�錾
void InitMagma();
void InitMagmaState();
void UpdateMagma();
void DrawMagma();
void ExitMagma();
int MagmaMax();

// �ʒu�擾
DirectX::XMFLOAT3 GetPositionMagma(int idx);
// �p���擾
DirectX::XMFLOAT3 GetRotationMagma(int idx);
// �ړ��ʎ擾
DirectX::XMFLOAT3 GetMoveMagma(int idx);