#pragma once
#include	<directxmath.h>
#include	"3DModel.h"
#include	"CPlaneMesh.h"
#include	"CMeshRenderer.h"

// �v���g�^�C�v�錾

void InitSlime();
void InitSlimeState();
void UpdateSlime();
void DrawSlime();
void ExitSlime();
int SlimeMax();

// �ʒu�擾
DirectX::XMFLOAT3 GetPositionSlime(int idx);
// �p���擾
DirectX::XMFLOAT3 GetRotationSlime(int idx);
// �ړ��ʎ擾
DirectX::XMFLOAT3 GetMoveSlime(int idx);