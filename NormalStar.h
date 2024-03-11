#pragma once
#include	<directxmath.h>
#include	"3DModel.h"
#include	"CBoundingSphere.h"
#include	"CShader.h"
#include	"CStaticMesh.h"
#include	"CStaticMeshRenderer.h"

struct CNormalStar {
	// �v���C���̏��(S,R,T)-----------------------------------------------------------------
	DirectX::SimpleMath::Vector3 position = { 0.0f,0.0f,0.0f };		// �ʒu
	DirectX::SimpleMath::Vector3 rotation = { 0.0f,0.0f,0.0f };		// �p��
	DirectX::SimpleMath::Vector3 scale = { 0.0f,0.0f,0.0f };		// �g�k
	DirectX::SimpleMath::Vector3 move = { 0.0f,0.0f,0.0f };			// �ړ���
	DirectX::SimpleMath::Vector3 destrot = { 0.0f,0.0f,0.0f };		// �ڕW��]�p�x
	//---------------------------------------------------------------------------------------
	// �v���g�^�C�v�錾

	void Init();		//������
	void Update();		//�X�V
	void Draw();		//�`��
	void Exit();		//���������
};
//-------------------------------------------
//���擾
BoundingSphere GetNormalStarBS();
CNormalStar* GetNormalStar();
//-------------------------------------------