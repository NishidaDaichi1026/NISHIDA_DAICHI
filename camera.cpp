#include "renderer.h"
#include "camera.h"
#include "CDirectInput.h"
#include "game.h"
#include "Player.h"
#include "sceneChange.h"
#include "weapon.h"
#include "BossEnemy.h"
#include "TrainingRoomFloor.h"
#include "Field.h"

using namespace DirectX::SimpleMath;

//*****************************************************************************
// �}�N����`
//*****************************************************************************
constexpr float VIEW_ANGLE = DirectX::XM_PI / 4.0f;							// ����p
constexpr float VIEW_ASPECT = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;	// �r���[���ʂ̃A�X�y�N�g��	
constexpr float VIEW_NEAR_Z = 1.0f;											// �r���[���ʂ�NearZ�l
constexpr float VIEW_FAR_Z = 2000.0f;										// �r���[���ʂ�FarZ�l
constexpr float VALUE_MOVE_CAMERA = 2.0f;									// �J�����̈ړ���
constexpr float VALUE_ROTATE_CAMERA = DirectX::XM_PI * 0.01f;				// �J�����̉�]��

constexpr float CAM_POS_P_X = 0.0f;			// �J�����̎��_�����ʒu(X���W)
constexpr float CAM_POS_P_Y = 10.0f;		// �J�����̎��_�����ʒu(Y���W)
constexpr float CAM_POS_P_Z = 80.0f;		// �J�����̎��_�����ʒu(Z���W)
constexpr float CAM_POS_LOOKAT_X = 0.0f;	// �J�����̒����_�����ʒu(X���W)
constexpr float CAM_POS_LOOKAT_Y = 0.0f;	// �J�����̒����_�����ʒu(Y���W)
constexpr float CAM_POS_LOOKAT_Z = 0.0f;	// �J�����̒����_�����ʒu(Z���W)

constexpr float	INTERVAL_CAMERA_R = 12.5f;		// ���f���̎����̐�܂ł̋���
constexpr float	RATE_CHASE_CAMERA_P = 0.30f;	// �J�����̎��_�ւ̕␳�W��
constexpr float	RATE_CHASE_CAMERA_R = 0.20f;	// �J�����̒����_�ւ̕␳�W��
constexpr float	INTERVAL_CAMERA_R_MOVE = 20.5f;	// ���f���̎����̐�܂ł̋���

constexpr float	CHASE_HEIGHT_P = 10.0f;			// �ǐՎ��̎��_�̍���
constexpr float	CHASE_HEIGHT_R = 0.0f;			// �ǐՎ��̒����_�̍���

// static �ϐ�--------------------------------------------------------------------------------------------------------------------------------

DirectX::SimpleMath::Vector3	Camera::m_Position = DirectX::SimpleMath::Vector3(CAM_POS_P_X, CAM_POS_P_Y, CAM_POS_P_Z);
DirectX::SimpleMath::Vector3	Camera::m_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
DirectX::SimpleMath::Vector3	Camera::m_Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);;

DirectX::SimpleMath::Vector3	Camera::m_Target = DirectX::SimpleMath::Vector3(CAM_POS_LOOKAT_X, CAM_POS_LOOKAT_Y, CAM_POS_LOOKAT_Z);
DirectX::SimpleMath::Matrix		Camera::m_ViewMatrix{};
DirectX::SimpleMath::Vector3	Camera::m_TargetDest= DirectX::SimpleMath::Vector3(CAM_POS_LOOKAT_X, CAM_POS_LOOKAT_Y, CAM_POS_LOOKAT_Z);
DirectX::SimpleMath::Vector3	Camera::m_PositionDest = DirectX::SimpleMath::Vector3(CAM_POS_P_X, CAM_POS_P_Y, CAM_POS_P_Z);

//�J�����̏��--------------------------------------------------------------------------------------------------------------------------------

DirectX::XMFLOAT3	posModel;				// ���f�����W
DirectX::XMFLOAT3	rotModel;				// ���f���p��
DirectX::XMFLOAT3	moveModel;				// ���f���ړ���
DirectX::XMFLOAT3   scale(1, 1, 1);			// �}�g���N�X�Ɏg�p 
DirectX::XMFLOAT3   trans(0, 0, 0);			// �}�g���N�X�Ɏg�p

//�J�����Ɏg���ϐ�----------------------------------------------------------------------------------------------------------------------------

float Camera::m_TargetLength = 0.0f;			// �����_�Ƃ̋���
float Camera::m_HeightCameraP;					// �ǔ����鎞�̈ʒu�i�����j
float Camera::m_HeightCameraR;					// �ǔ����鎞�̒����_�̈ʒu�i�����j

float		fIntervalCamera;				// �J�����ƃ��f���̋���
float		fLengthMove;					// �ړ���
float		fRateChaseCameraPos;			// �J�����ʒu�̈ړ���
float		fRateChaseCameraLookat;			// �����_�̈ړ���
float		XZ_TargetLength;				// �J������Y�ړ������Ƃ���XZ��Z������i�J�������񂹂�j
float		cameralimit = 10.0f;

int mouseIniPosY = (int)SCREEN_WIDTH / 2;		// �}�E�X�J�[�\���̏����ʒu
int mouseIniPosX = (int)SCREEN_HEIGHT / 2;		// �}�E�X�J�[�\���̏����ʒu

int mouseNowPosX = 0;						// ���݂̃}�E�X�J�[�\���̈ʒu
int mouseNowPosY = 0;						// ���݂̃}�E�X�J�[�\���̈ʒu

int camSensX = 10;							//�J�������xX
int camSensY = 5;							//�J�������xY

float ShakeCameraWidth = 10.0f;
float ReduceShake = 0.9f;
float ShakeCameraCnt = 0.0f;

//--------------------------------------------------------------------------------------------------------------------------------------------
TRoom c_troom;
Field c_field;
//--------------------------------------------------------------------------------------------------------------------------------------------
//������
void Camera::Init()
{
	// �J�����ƒ����_�̋��������߂�
	m_TargetLength = (m_Position - m_Target).Length();
	// �J�����ƒ����_�̋�����ۑ�
	XZ_TargetLength = m_TargetLength;
	// �ǔ����鎞�̈ʒu�i�����j
	m_HeightCameraP = CHASE_HEIGHT_P;
	// �ǔ����鎞�̒����_�̈ʒu�i�����j
	m_HeightCameraR = CHASE_HEIGHT_R;
	
	CDirectInput::GetInstance().SetCursorPosition();
}

//�o�g���V�[���p�J����
void Camera::UpdateBS(){

	//���擾------------------------------------------------------------------------------------------------

	posModel = GetPositionPlayer();				// �v���C���̈ʒu���擾
	rotModel = GetRotationPlayer();				// �v���C���̌����擾
	moveModel = GetMovePlayer();				// �v���C���̈ړ��ʎ擾

	Weapon_ST* weapon_st = GetWeaponST();

	//--------------------------------------------------------------------------------------------------------

	float height;

	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, scale, rotModel, trans);

	// ���f�����ړ������H
	if (moveModel.x < -0.05f || moveModel.x > 0.05f
		|| moveModel.z < -0.05f || moveModel.z > 0.05f)
	{
		fIntervalCamera = INTERVAL_CAMERA_R + sqrtf(moveModel.x * moveModel.x + moveModel.z * moveModel.z) * 10.0f;
		fRateChaseCameraLookat = 0.10f;
		fRateChaseCameraPos = 0.030f;
	}
	else
	{
		// ���f������~��
		fIntervalCamera = INTERVAL_CAMERA_R + sqrtf(moveModel.x * moveModel.x + moveModel.z * moveModel.z) * 6.0f;
		fRateChaseCameraLookat = 0.005f;
		fRateChaseCameraPos = 0.030f;
	}
	fLengthMove = sqrtf(moveModel.x * moveModel.x + moveModel.z * moveModel.z) * 6.0f;

	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;

	//���C�g����Ɏ��_�������Ƃ炷�悤�ɂ���------------------------------------------------------------------

	LIGHT light{};
	light.Enable = true;
	light.Direction = Vector4(m_ViewMatrix._13, m_ViewMatrix._23, m_ViewMatrix._33, 0.0f);
	light.Direction.Normalize();
	light.Ambient = Color(0.2f, 0.2f, 0.2f, 1.0f);
	light.Diffuse = Color(1.5f, 1.5f, 1.5f, 1.0f);
	Renderer::SetLight(light);

	// �����_�̖ڕW�ʒu---------------------------------------------------------------------------------------

	m_TargetDest.x = posModel.x - sin(rotModel.y) * fIntervalCamera;
	m_TargetDest.y = posModel.y - m_HeightCameraR + 10;
	m_TargetDest.z = posModel.z - cos(rotModel.y) * fIntervalCamera;

	// ���_�̖ڕW�ʒu-----------------------------------------------------------------------------------------
	
	m_PositionDest.x = posModel.x - sinf(m_Rotation.y) * m_TargetLength - sin(rotModel.y) * fLengthMove;
	m_PositionDest.y = posModel.y + m_HeightCameraP;
	m_PositionDest.z = posModel.z - cosf(m_Rotation.y) * m_TargetLength - cos(rotModel.y) * fLengthMove;

	// �����_�̕␳-------------------------------------------------------------------------------------------
	
	m_Target = posModel;
	
	m_Target.x += (m_TargetDest.x - m_Target.x) * fRateChaseCameraLookat;
	m_Target.y += (m_TargetDest.y - m_Target.y) * 0.10f;
	m_Target.z += (m_TargetDest.z - m_Target.z) * fRateChaseCameraLookat;

	m_Target.y = m_Target.y + 20.0f;
	
	// ���_�̕␳---------------------------------------------------------------------------------------------

	if (weapon_st->attTime == 0) {
		m_Position += (m_PositionDest - m_Position) * fRateChaseCameraPos;
	}

	//�J������Y��]�@�}�E�X����-------------------------------------------------------------------------------

	mouseNowPosY = CDirectInput::GetInstance().GetMousePosX();
	int camRotY = mouseNowPosY - mouseIniPosY;
	camRotY = (camRotY / camSensY) % 360;
	m_Rotation.y = camRotY * VALUE_ROTATE_CAMERA;

	m_Position.x = posModel.x - sinf(m_Rotation.y) * XZ_TargetLength;
	m_Position.z = posModel.z - cosf(m_Rotation.y) * XZ_TargetLength;
	
	//�J������X��]�@�}�E�X����-------------------------------------------------------------------------------

	mouseNowPosX = CDirectInput::GetInstance().GetMousePosY();
	//�}�E�X�J�[�\���̏����ʒu���猻�݂̈ʒu������
	int camRotX = mouseNowPosX - mouseIniPosX;
	//�}�E�X���x�Ŋ���
	camRotX = (camRotX / camSensX) % 360;
	
	m_Rotation.x = camRotX * VALUE_ROTATE_CAMERA;

	//�J�������㉺������ƃJ������XZ�����v���C���[�Ɋ񂹂�
	if (camRotX < 0) {
		XZ_TargetLength = (m_TargetLength + camRotX);
	}
	if (camRotX > 0) {
		XZ_TargetLength = (m_TargetLength - camRotX);
	}
	
	m_Position.y = posModel.y + sinf(m_Rotation.x) * m_TargetLength;

	ChargeAnime();

	ShakeCamera();

	//--------------------------------------------------------------------------------------------------------
	// �͈̓`�F�b�N
	if (ReturnSceneNum() == 3) {
		//�g���[�j���O���[���@�͈̓`�F�b�N�@�J�����ʒu
		if (m_Position.x <= -c_troom.TRoomX_MAX / 2) {
			m_Position.x = -c_troom.TRoomX_MAX / 2 + cameralimit;
		}
		if (m_Position.x >= c_troom.TRoomX_MAX / 2) {
			m_Position.x = c_troom.TRoomX_MAX / 2 - cameralimit;
		}
		if (m_Position.z >= c_troom.TRoomZ_MAX / 2) {
			m_Position.z = c_troom.TRoomZ_MAX / 2 - cameralimit;
		}
		if (m_Position.z <= -c_troom.TRoomZ_MAX / 2) {
			m_Position.z = -c_troom.TRoomZ_MAX / 2 + cameralimit;
		}
		// �͈̓`�F�b�N�@�����_�ʒu
		if (m_Target.x <= -c_troom.TRoomX_MAX / 2) {
			m_Target.x = -c_troom.TRoomX_MAX / 2 + cameralimit;
		}
		if (m_Target.x >= c_troom.TRoomX_MAX / 2) {
			m_Target.x = c_troom.TRoomX_MAX / 2 - cameralimit;
		}
		if (m_Target.z >= c_troom.TRoomZ_MAX / 2) {
			m_Target.z = c_troom.TRoomZ_MAX / 2 - cameralimit;
		}
		if (m_Target.z <= -c_troom.TRoomZ_MAX / 2) {
			m_Target.z = -c_troom.TRoomZ_MAX / 2 + cameralimit;
		}

		height = GetTRoomHeight(m_Position);
	}
	else {
		//�t�B�[���h�@�͈̓`�F�b�N�@�J�����ʒu
		if (m_Position.x <= -c_field.FieldX_MAX / 2) {
			m_Position.x = -c_field.FieldX_MAX / 2 + cameralimit;
		}
		if (m_Position.x >= c_field.FieldX_MAX / 2) {
			m_Position.x = c_field.FieldX_MAX / 2 - cameralimit;
		}
		if (m_Position.z >= c_field.FieldZ_MAX / 2) {
			m_Position.z = c_field.FieldZ_MAX / 2 - cameralimit;
		}
		if (m_Position.z <= -c_field.FieldZ_MAX / 2) {
			m_Position.z = -c_field.FieldZ_MAX / 2 - cameralimit;
		}
		// �͈̓`�F�b�N�@�����_�ʒu
		if (m_Target.x <= -c_field.FieldX_MAX / 2) {
			m_Target.x = -c_field.FieldX_MAX / 2 + cameralimit;
		}
		if (m_Target.x >= c_field.FieldX_MAX / 2) {
			m_Target.x = c_field.FieldX_MAX / 2 - cameralimit;
		}
		if (m_Target.z >= c_field.FieldZ_MAX / 2) {
			m_Target.z = c_field.FieldZ_MAX / 2 - cameralimit;
		}
		if (m_Target.z <= -c_field.FieldZ_MAX / 2) {
			m_Target.z = -c_field.FieldZ_MAX / 2 - cameralimit;
		}

		height = GetFieldHeight(m_Position);
	}
	if (m_Position.y <= height + 2.0f) {
		m_Position.y = height + 2.0f;
	}
}

//�{�X�V�[���p�J�����i�ǔ��j
void Camera::UpdateBossS() {

	//���擾------------------------------------------------------------------------------------------------

	posModel = GetPositionPlayer();				// �v���C���̈ʒu���擾
	rotModel = GetRotationPlayer();				// �v���C���̌����擾
	moveModel = GetMovePlayer();				// �v���C���̈ړ��ʎ擾

	Weapon_ST* weapon_st = GetWeaponST();

	//--------------------------------------------------------------------------------------------------------

	float height;

	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, scale, rotModel, trans);

	// ���f�����ړ������H
	if (moveModel.x < -0.05f || moveModel.x > 0.05f
		|| moveModel.z < -0.05f || moveModel.z > 0.05f)
	{
		fIntervalCamera = INTERVAL_CAMERA_R + sqrtf(moveModel.x * moveModel.x + moveModel.z * moveModel.z) * 10.0f;
		fRateChaseCameraLookat = 0.10f;
		fRateChaseCameraPos = 0.030f;
	}
	else
	{
		// ���f������~��
		fIntervalCamera = INTERVAL_CAMERA_R + sqrtf(moveModel.x * moveModel.x + moveModel.z * moveModel.z) * 6.0f;
		fRateChaseCameraLookat = 0.005f;
		fRateChaseCameraPos = 0.030f;
	}
	fLengthMove = sqrtf(moveModel.x * moveModel.x + moveModel.z * moveModel.z) * 6.0f;

	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;

	//���C�g����Ɏ��_�������Ƃ炷�悤�ɂ���------------------------------------------------------------------

	LIGHT light{};
	light.Enable = true;
	light.Direction = Vector4(m_ViewMatrix._13, m_ViewMatrix._23, m_ViewMatrix._33, 0.0f);
	light.Direction.Normalize();
	light.Ambient = Color(0.2f, 0.2f, 0.2f, 1.0f);
	light.Diffuse = Color(1.5f, 1.5f, 1.5f, 1.0f);
	Renderer::SetLight(light);

	// �����_�̖ڕW�ʒu---------------------------------------------------------------------------------------

	m_Target = GetBossPosition();	//�{�X�ɒ����_�������邽��

	m_TargetDest.x = m_Target.x - sin(rotModel.y) * fIntervalCamera;
	m_TargetDest.y = m_Target.y - m_HeightCameraR + 10;
	m_TargetDest.z = m_Target.z - cos(rotModel.y) * fIntervalCamera;

	// ���_�̖ڕW�ʒu-----------------------------------------------------------------------------------------

	m_PositionDest.x = m_Target.x - sinf(m_Rotation.y) * m_TargetLength - sin(rotModel.y) * fLengthMove;
	m_PositionDest.y = m_Target.y + m_HeightCameraP;
	m_PositionDest.z = m_Target.z - cosf(m_Rotation.y) * m_TargetLength - cos(rotModel.y) * fLengthMove;

	// �����_�̕␳-------------------------------------------------------------------------------------------

	m_Target.x += (m_TargetDest.x - m_Target.x) * fRateChaseCameraLookat;
	m_Target.y += (m_TargetDest.y - m_Target.y) * 0.10f;
	m_Target.z += (m_TargetDest.z - m_Target.z) * fRateChaseCameraLookat;

	m_Target.y = m_Target.y + 20.0f;

	// ���_�̕␳---------------------------------------------------------------------------------------------

	if (weapon_st->attTime == 0) {
		m_Position += (m_PositionDest - m_Position) * fRateChaseCameraPos;
	}

	//�J������Y��]�@�}�E�X����-------------------------------------------------------------------------------

	mouseNowPosY = CDirectInput::GetInstance().GetMousePosX();
	int camRotY = mouseNowPosY - mouseIniPosY;
	camRotY = (camRotY / camSensY) % 360;
	m_Rotation.y = camRotY * VALUE_ROTATE_CAMERA;

	m_Position.x = posModel.x - sinf(m_Rotation.y) * XZ_TargetLength * 1.6f;
	m_Position.z = posModel.z - cosf(m_Rotation.y) * XZ_TargetLength * 1.6f;

	//�J������X��]�@�}�E�X����-------------------------------------------------------------------------------

	mouseNowPosX = CDirectInput::GetInstance().GetMousePosY();
	//�}�E�X�J�[�\���̏����ʒu���猻�݂̈ʒu������
	int camRotX = mouseNowPosX - mouseIniPosX;

	camRotX = (camRotX / camSensX) % 360;

	m_Rotation.x = camRotX * VALUE_ROTATE_CAMERA;

	//�J�������㉺������ƃJ������XZ�����v���C���[�Ɋ񂹂�
	if (camRotX < 0) {
		XZ_TargetLength = (m_TargetLength + camRotX);
	}
	if (camRotX > 0) {
		XZ_TargetLength = (m_TargetLength - camRotX);
	}

	m_Position.y = posModel.y + sinf(m_Rotation.x) * m_TargetLength;

	ChargeAnime();

	ShakeCamera();

	//--------------------------------------------------------------------------------------------------------
	// �͈̓`�F�b�N�@�J�����ʒu
	if (m_Position.x <= -c_field.FieldX_MAX / 2) {
		m_Position.x = -c_field.FieldX_MAX / 2 + cameralimit;
	}
	if (m_Position.x >= c_field.FieldX_MAX / 2) {
		m_Position.x = c_field.FieldX_MAX / 2 - cameralimit;
	}
	if (m_Position.z >= c_field.FieldZ_MAX / 2) {
		m_Position.z = c_field.FieldZ_MAX / 2 - cameralimit;
	}
	if (m_Position.z <= -c_field.FieldZ_MAX / 2) {
		m_Position.z = -c_field.FieldZ_MAX / 2 - cameralimit;
	}
	// �͈̓`�F�b�N�@�����_�ʒu
	if (m_Target.x <= -c_field.FieldX_MAX / 2) {
		m_Target.x = -c_field.FieldX_MAX / 2 + cameralimit;
	}
	if (m_Target.x >= c_field.FieldX_MAX / 2) {
		m_Target.x = c_field.FieldX_MAX / 2 - cameralimit;
	}
	if (m_Target.z >= c_field.FieldZ_MAX / 2) {
		m_Target.z = c_field.FieldZ_MAX / 2 - cameralimit;
	}
	if (m_Target.z <= -c_field.FieldZ_MAX / 2) {
		m_Target.z = -c_field.FieldZ_MAX / 2 - cameralimit;
	}
	height = GetFieldHeight(m_Position);
	if (m_Position.y <= height + 2.0f) {
		m_Position.y = height + 2.0f;
	}
}

void Camera::ChargeAnime() {
	Weapon_ST* weapon_st = GetWeaponST();
	if (CDirectInput::GetInstance().GetMouseRButtonCheck()) {
		XZ_TargetLength = XZ_TargetLength + weapon_st->charge;		//�`���[�W�����񂾂�ƃJ�����𗣂�
	}
}

void Camera::ShakeCamera() {
	Player* g_player = GetPlayer();
	if (g_player->ThunderSkillChack) {		//���̃X�L�����g������
		ShakeCameraCnt++;					//�J�E���g�A�b�v
		ShakeCameraWidth *= ReduceShake;	//��ʂ̐U�ꕝ�����񂾂񏬂���
	}
	//�ӂ蕝���������Ȃ����烊�Z�b�g
	if (ShakeCameraWidth <= 0.1f) {
		ShakeCameraCnt = 0.0f;
		ShakeCameraWidth = 10.0f;
		g_player->ThunderSkillChack = false;
	}

	//�J�����|�W�V����
	m_Position.x = m_Position.x + sinf(ShakeCameraCnt) * (ShakeCameraWidth);
	m_Position.y = m_Position.y + sinf(ShakeCameraCnt) * (ShakeCameraWidth);
	//�����_�|�W�V����
	m_Target.x = m_Target.x + sinf(ShakeCameraCnt) * (ShakeCameraWidth);
	m_Target.y = m_Target.y + sinf(ShakeCameraCnt) * (ShakeCameraWidth);
}

//�`��p
void Camera::Draw()
{
	// �r���[�ϊ����쐬
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
	m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_Position, m_Target, up);

	Renderer::SetViewMatrix(&m_ViewMatrix);

	//�v���W�F�N�V�����s��̐���
	float fieldOfView = VIEW_ANGLE;    // ����p
	
	float aspectRatio = VIEW_ASPECT;	// �A�X�y�N�g��	
	float nearPlane = VIEW_NEAR_Z;       // �j�A�N���b�v
	float farPlane = VIEW_FAR_Z;      // �t�@�[�N���b�v

	//�v���W�F�N�V�����s��̐���
	Matrix projectionMatrix;
	projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearPlane, farPlane);	// ����n�ɂ����@20230511 by suzuki.tomoki


	Renderer::SetProjectionMatrix(&projectionMatrix);
}

//�QD�`��p
void Camera::Draw2D()
{
	// ���[���h�ϊ��s��Z�b�g
	Matrix world;
	world = Matrix::Identity;			// �P�ʍs��ɂ���
	Renderer::SetWorldMatrix(&world);

	// �r���[�ϊ��s��Z�b�g
	Matrix view;
	view = Matrix::Identity;
	Renderer::SetViewMatrix(&view);

	Matrix projection;

	// 2D�`������㌴�_�ɂ���  (20230512 update by tomoki.suzuki�@
	projection = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f,
		static_cast<float>(SCREEN_WIDTH),		// �r���[�{�����[���̍ŏ��w
		static_cast<float>(SCREEN_HEIGHT),		// �r���[�{�����[���̍ŏ��x
		0.0f,									// �r���[�{�����[���̍ő�Y
		0.0f,
		1.0f);

	Renderer::SetProjectionMatrix(&projection);

}