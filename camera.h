#pragma once

#include	<SimpleMath.h>

class Camera {
private:
	//�J�����̏��-----------------------------------------------------------

	static DirectX::SimpleMath::Vector3	m_Position;
	static DirectX::SimpleMath::Vector3	m_Rotation;
	static DirectX::SimpleMath::Vector3	m_Scale;

	//�J�����Ɏg�p����ϐ�---------------------------------------------------

	static DirectX::SimpleMath::Vector3	m_Target;		//�����_
	static DirectX::SimpleMath::Matrix	m_ViewMatrix;	//�r���[�}�g���N�X

	static float m_TargetLength;						//�����_�Ƃ̋���

	static DirectX::SimpleMath::Vector3 m_TargetDest;	//�����_�̖ڕW�ʒu
	static DirectX::SimpleMath::Vector3 m_PositionDest;	//���_�̖ڕW�ʒu

	static float m_HeightCameraP;						//�ǔ����鎞�̈ʒu�i�����j
	static float m_HeightCameraR;						//�ǔ����鎞�̒����_�̈ʒu�i�����j

	//-----------------------------------------------------------------------

public:

	//�v���g�^�C�v�錾-------------------------------------------------------

	//������
	static void Init();
	//�o�g���V�[���p�J����
	static void UpdateBS();
	//�{�X�V�[���p�J����
	static void UpdateBossS();
	//�`���[�W���̃J�����A�j���[�V����
	static void ChargeAnime();
	//�J������h�炷
	static void ShakeCamera();
	//�`��p
	static void Draw();
	//2D�`��p
	static void Draw2D();

	//���擾--------------------------------------------------------------

	static DirectX::SimpleMath::Vector3 GetRotCamera() {
		return m_Rotation;
	}
	static DirectX::SimpleMath::Vector3 GetPosCamera() {
		return m_Position;
	}
	static DirectX::SimpleMath::Vector3 GetTargetCamera() {
		return m_Target;
	}
	static DirectX::SimpleMath::Matrix GetViewMtx() {
		return m_ViewMatrix;
	}
	
	//----------------------------------------------------------------------
};