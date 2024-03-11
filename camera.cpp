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
// マクロ定義
//*****************************************************************************
constexpr float VIEW_ANGLE = DirectX::XM_PI / 4.0f;							// 視野角
constexpr float VIEW_ASPECT = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;	// ビュー平面のアスペクト比	
constexpr float VIEW_NEAR_Z = 1.0f;											// ビュー平面のNearZ値
constexpr float VIEW_FAR_Z = 2000.0f;										// ビュー平面のFarZ値
constexpr float VALUE_MOVE_CAMERA = 2.0f;									// カメラの移動量
constexpr float VALUE_ROTATE_CAMERA = DirectX::XM_PI * 0.01f;				// カメラの回転量

constexpr float CAM_POS_P_X = 0.0f;			// カメラの視点初期位置(X座標)
constexpr float CAM_POS_P_Y = 10.0f;		// カメラの視点初期位置(Y座標)
constexpr float CAM_POS_P_Z = 80.0f;		// カメラの視点初期位置(Z座標)
constexpr float CAM_POS_LOOKAT_X = 0.0f;	// カメラの注視点初期位置(X座標)
constexpr float CAM_POS_LOOKAT_Y = 0.0f;	// カメラの注視点初期位置(Y座標)
constexpr float CAM_POS_LOOKAT_Z = 0.0f;	// カメラの注視点初期位置(Z座標)

constexpr float	INTERVAL_CAMERA_R = 12.5f;		// モデルの視線の先までの距離
constexpr float	RATE_CHASE_CAMERA_P = 0.30f;	// カメラの視点への補正係数
constexpr float	RATE_CHASE_CAMERA_R = 0.20f;	// カメラの注視点への補正係数
constexpr float	INTERVAL_CAMERA_R_MOVE = 20.5f;	// モデルの視線の先までの距離

constexpr float	CHASE_HEIGHT_P = 10.0f;			// 追跡時の視点の高さ
constexpr float	CHASE_HEIGHT_R = 0.0f;			// 追跡時の注視点の高さ

// static 変数--------------------------------------------------------------------------------------------------------------------------------

DirectX::SimpleMath::Vector3	Camera::m_Position = DirectX::SimpleMath::Vector3(CAM_POS_P_X, CAM_POS_P_Y, CAM_POS_P_Z);
DirectX::SimpleMath::Vector3	Camera::m_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
DirectX::SimpleMath::Vector3	Camera::m_Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);;

DirectX::SimpleMath::Vector3	Camera::m_Target = DirectX::SimpleMath::Vector3(CAM_POS_LOOKAT_X, CAM_POS_LOOKAT_Y, CAM_POS_LOOKAT_Z);
DirectX::SimpleMath::Matrix		Camera::m_ViewMatrix{};
DirectX::SimpleMath::Vector3	Camera::m_TargetDest= DirectX::SimpleMath::Vector3(CAM_POS_LOOKAT_X, CAM_POS_LOOKAT_Y, CAM_POS_LOOKAT_Z);
DirectX::SimpleMath::Vector3	Camera::m_PositionDest = DirectX::SimpleMath::Vector3(CAM_POS_P_X, CAM_POS_P_Y, CAM_POS_P_Z);

//カメラの情報--------------------------------------------------------------------------------------------------------------------------------

DirectX::XMFLOAT3	posModel;				// モデル座標
DirectX::XMFLOAT3	rotModel;				// モデル姿勢
DirectX::XMFLOAT3	moveModel;				// モデル移動量
DirectX::XMFLOAT3   scale(1, 1, 1);			// マトリクスに使用 
DirectX::XMFLOAT3   trans(0, 0, 0);			// マトリクスに使用

//カメラに使う変数----------------------------------------------------------------------------------------------------------------------------

float Camera::m_TargetLength = 0.0f;			// 注視点との距離
float Camera::m_HeightCameraP;					// 追尾する時の位置（高さ）
float Camera::m_HeightCameraR;					// 追尾する時の注視点の位置（高さ）

float		fIntervalCamera;				// カメラとモデルの距離
float		fLengthMove;					// 移動量
float		fRateChaseCameraPos;			// カメラ位置の移動率
float		fRateChaseCameraLookat;			// 注視点の移動率
float		XZ_TargetLength;				// カメラがY移動したときにXZを短くする（カメラを寄せる）
float		cameralimit = 10.0f;

int mouseIniPosY = (int)SCREEN_WIDTH / 2;		// マウスカーソルの初期位置
int mouseIniPosX = (int)SCREEN_HEIGHT / 2;		// マウスカーソルの初期位置

int mouseNowPosX = 0;						// 現在のマウスカーソルの位置
int mouseNowPosY = 0;						// 現在のマウスカーソルの位置

int camSensX = 10;							//カメラ感度X
int camSensY = 5;							//カメラ感度Y

float ShakeCameraWidth = 10.0f;
float ReduceShake = 0.9f;
float ShakeCameraCnt = 0.0f;

//--------------------------------------------------------------------------------------------------------------------------------------------
TRoom c_troom;
Field c_field;
//--------------------------------------------------------------------------------------------------------------------------------------------
//初期化
void Camera::Init()
{
	// カメラと注視点の距離を求める
	m_TargetLength = (m_Position - m_Target).Length();
	// カメラと注視点の距離を保存
	XZ_TargetLength = m_TargetLength;
	// 追尾する時の位置（高さ）
	m_HeightCameraP = CHASE_HEIGHT_P;
	// 追尾する時の注視点の位置（高さ）
	m_HeightCameraR = CHASE_HEIGHT_R;
	
	CDirectInput::GetInstance().SetCursorPosition();
}

//バトルシーン用カメラ
void Camera::UpdateBS(){

	//情報取得------------------------------------------------------------------------------------------------

	posModel = GetPositionPlayer();				// プレイヤの位置を取得
	rotModel = GetRotationPlayer();				// プレイヤの向き取得
	moveModel = GetMovePlayer();				// プレイヤの移動量取得

	Weapon_ST* weapon_st = GetWeaponST();

	//--------------------------------------------------------------------------------------------------------

	float height;

	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, scale, rotModel, trans);

	// モデルが移動中か？
	if (moveModel.x < -0.05f || moveModel.x > 0.05f
		|| moveModel.z < -0.05f || moveModel.z > 0.05f)
	{
		fIntervalCamera = INTERVAL_CAMERA_R + sqrtf(moveModel.x * moveModel.x + moveModel.z * moveModel.z) * 10.0f;
		fRateChaseCameraLookat = 0.10f;
		fRateChaseCameraPos = 0.030f;
	}
	else
	{
		// モデルが停止中
		fIntervalCamera = INTERVAL_CAMERA_R + sqrtf(moveModel.x * moveModel.x + moveModel.z * moveModel.z) * 6.0f;
		fRateChaseCameraLookat = 0.005f;
		fRateChaseCameraPos = 0.030f;
	}
	fLengthMove = sqrtf(moveModel.x * moveModel.x + moveModel.z * moveModel.z) * 6.0f;

	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;

	//ライトが常に視点方向を照らすようにする------------------------------------------------------------------

	LIGHT light{};
	light.Enable = true;
	light.Direction = Vector4(m_ViewMatrix._13, m_ViewMatrix._23, m_ViewMatrix._33, 0.0f);
	light.Direction.Normalize();
	light.Ambient = Color(0.2f, 0.2f, 0.2f, 1.0f);
	light.Diffuse = Color(1.5f, 1.5f, 1.5f, 1.0f);
	Renderer::SetLight(light);

	// 注視点の目標位置---------------------------------------------------------------------------------------

	m_TargetDest.x = posModel.x - sin(rotModel.y) * fIntervalCamera;
	m_TargetDest.y = posModel.y - m_HeightCameraR + 10;
	m_TargetDest.z = posModel.z - cos(rotModel.y) * fIntervalCamera;

	// 視点の目標位置-----------------------------------------------------------------------------------------
	
	m_PositionDest.x = posModel.x - sinf(m_Rotation.y) * m_TargetLength - sin(rotModel.y) * fLengthMove;
	m_PositionDest.y = posModel.y + m_HeightCameraP;
	m_PositionDest.z = posModel.z - cosf(m_Rotation.y) * m_TargetLength - cos(rotModel.y) * fLengthMove;

	// 注視点の補正-------------------------------------------------------------------------------------------
	
	m_Target = posModel;
	
	m_Target.x += (m_TargetDest.x - m_Target.x) * fRateChaseCameraLookat;
	m_Target.y += (m_TargetDest.y - m_Target.y) * 0.10f;
	m_Target.z += (m_TargetDest.z - m_Target.z) * fRateChaseCameraLookat;

	m_Target.y = m_Target.y + 20.0f;
	
	// 視点の補正---------------------------------------------------------------------------------------------

	if (weapon_st->attTime == 0) {
		m_Position += (m_PositionDest - m_Position) * fRateChaseCameraPos;
	}

	//カメラのY回転　マウス操作-------------------------------------------------------------------------------

	mouseNowPosY = CDirectInput::GetInstance().GetMousePosX();
	int camRotY = mouseNowPosY - mouseIniPosY;
	camRotY = (camRotY / camSensY) % 360;
	m_Rotation.y = camRotY * VALUE_ROTATE_CAMERA;

	m_Position.x = posModel.x - sinf(m_Rotation.y) * XZ_TargetLength;
	m_Position.z = posModel.z - cosf(m_Rotation.y) * XZ_TargetLength;
	
	//カメラのX回転　マウス操作-------------------------------------------------------------------------------

	mouseNowPosX = CDirectInput::GetInstance().GetMousePosY();
	//マウスカーソルの初期位置から現在の位置を引く
	int camRotX = mouseNowPosX - mouseIniPosX;
	//マウス感度で割る
	camRotX = (camRotX / camSensX) % 360;
	
	m_Rotation.x = camRotX * VALUE_ROTATE_CAMERA;

	//カメラを上下させるとカメラのXZ軸をプレイヤーに寄せる
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
	// 範囲チェック
	if (ReturnSceneNum() == 3) {
		//トレーニングルーム　範囲チェック　カメラ位置
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
		// 範囲チェック　注視点位置
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
		//フィールド　範囲チェック　カメラ位置
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
		// 範囲チェック　注視点位置
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

//ボスシーン用カメラ（追尾）
void Camera::UpdateBossS() {

	//情報取得------------------------------------------------------------------------------------------------

	posModel = GetPositionPlayer();				// プレイヤの位置を取得
	rotModel = GetRotationPlayer();				// プレイヤの向き取得
	moveModel = GetMovePlayer();				// プレイヤの移動量取得

	Weapon_ST* weapon_st = GetWeaponST();

	//--------------------------------------------------------------------------------------------------------

	float height;

	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, scale, rotModel, trans);

	// モデルが移動中か？
	if (moveModel.x < -0.05f || moveModel.x > 0.05f
		|| moveModel.z < -0.05f || moveModel.z > 0.05f)
	{
		fIntervalCamera = INTERVAL_CAMERA_R + sqrtf(moveModel.x * moveModel.x + moveModel.z * moveModel.z) * 10.0f;
		fRateChaseCameraLookat = 0.10f;
		fRateChaseCameraPos = 0.030f;
	}
	else
	{
		// モデルが停止中
		fIntervalCamera = INTERVAL_CAMERA_R + sqrtf(moveModel.x * moveModel.x + moveModel.z * moveModel.z) * 6.0f;
		fRateChaseCameraLookat = 0.005f;
		fRateChaseCameraPos = 0.030f;
	}
	fLengthMove = sqrtf(moveModel.x * moveModel.x + moveModel.z * moveModel.z) * 6.0f;

	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;

	//ライトが常に視点方向を照らすようにする------------------------------------------------------------------

	LIGHT light{};
	light.Enable = true;
	light.Direction = Vector4(m_ViewMatrix._13, m_ViewMatrix._23, m_ViewMatrix._33, 0.0f);
	light.Direction.Normalize();
	light.Ambient = Color(0.2f, 0.2f, 0.2f, 1.0f);
	light.Diffuse = Color(1.5f, 1.5f, 1.5f, 1.0f);
	Renderer::SetLight(light);

	// 注視点の目標位置---------------------------------------------------------------------------------------

	m_Target = GetBossPosition();	//ボスに注視点を向けるため

	m_TargetDest.x = m_Target.x - sin(rotModel.y) * fIntervalCamera;
	m_TargetDest.y = m_Target.y - m_HeightCameraR + 10;
	m_TargetDest.z = m_Target.z - cos(rotModel.y) * fIntervalCamera;

	// 視点の目標位置-----------------------------------------------------------------------------------------

	m_PositionDest.x = m_Target.x - sinf(m_Rotation.y) * m_TargetLength - sin(rotModel.y) * fLengthMove;
	m_PositionDest.y = m_Target.y + m_HeightCameraP;
	m_PositionDest.z = m_Target.z - cosf(m_Rotation.y) * m_TargetLength - cos(rotModel.y) * fLengthMove;

	// 注視点の補正-------------------------------------------------------------------------------------------

	m_Target.x += (m_TargetDest.x - m_Target.x) * fRateChaseCameraLookat;
	m_Target.y += (m_TargetDest.y - m_Target.y) * 0.10f;
	m_Target.z += (m_TargetDest.z - m_Target.z) * fRateChaseCameraLookat;

	m_Target.y = m_Target.y + 20.0f;

	// 視点の補正---------------------------------------------------------------------------------------------

	if (weapon_st->attTime == 0) {
		m_Position += (m_PositionDest - m_Position) * fRateChaseCameraPos;
	}

	//カメラのY回転　マウス操作-------------------------------------------------------------------------------

	mouseNowPosY = CDirectInput::GetInstance().GetMousePosX();
	int camRotY = mouseNowPosY - mouseIniPosY;
	camRotY = (camRotY / camSensY) % 360;
	m_Rotation.y = camRotY * VALUE_ROTATE_CAMERA;

	m_Position.x = posModel.x - sinf(m_Rotation.y) * XZ_TargetLength * 1.6f;
	m_Position.z = posModel.z - cosf(m_Rotation.y) * XZ_TargetLength * 1.6f;

	//カメラのX回転　マウス操作-------------------------------------------------------------------------------

	mouseNowPosX = CDirectInput::GetInstance().GetMousePosY();
	//マウスカーソルの初期位置から現在の位置を引く
	int camRotX = mouseNowPosX - mouseIniPosX;

	camRotX = (camRotX / camSensX) % 360;

	m_Rotation.x = camRotX * VALUE_ROTATE_CAMERA;

	//カメラを上下させるとカメラのXZ軸をプレイヤーに寄せる
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
	// 範囲チェック　カメラ位置
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
	// 範囲チェック　注視点位置
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
		XZ_TargetLength = XZ_TargetLength + weapon_st->charge;		//チャージ中だんだんとカメラを離す
	}
}

void Camera::ShakeCamera() {
	Player* g_player = GetPlayer();
	if (g_player->ThunderSkillChack) {		//雷のスキルを使ったら
		ShakeCameraCnt++;					//カウントアップ
		ShakeCameraWidth *= ReduceShake;	//画面の振れ幅をだんだん小さく
	}
	//ふり幅が小さくなったらリセット
	if (ShakeCameraWidth <= 0.1f) {
		ShakeCameraCnt = 0.0f;
		ShakeCameraWidth = 10.0f;
		g_player->ThunderSkillChack = false;
	}

	//カメラポジション
	m_Position.x = m_Position.x + sinf(ShakeCameraCnt) * (ShakeCameraWidth);
	m_Position.y = m_Position.y + sinf(ShakeCameraCnt) * (ShakeCameraWidth);
	//注視点ポジション
	m_Target.x = m_Target.x + sinf(ShakeCameraCnt) * (ShakeCameraWidth);
	m_Target.y = m_Target.y + sinf(ShakeCameraCnt) * (ShakeCameraWidth);
}

//描画用
void Camera::Draw()
{
	// ビュー変換後列作成
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
	m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_Position, m_Target, up);

	Renderer::SetViewMatrix(&m_ViewMatrix);

	//プロジェクション行列の生成
	float fieldOfView = VIEW_ANGLE;    // 視野角
	
	float aspectRatio = VIEW_ASPECT;	// アスペクト比	
	float nearPlane = VIEW_NEAR_Z;       // ニアクリップ
	float farPlane = VIEW_FAR_Z;      // ファークリップ

	//プロジェクション行列の生成
	Matrix projectionMatrix;
	projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearPlane, farPlane);	// 左手系にした　20230511 by suzuki.tomoki


	Renderer::SetProjectionMatrix(&projectionMatrix);
}

//２D描画用
void Camera::Draw2D()
{
	// ワールド変換行列セット
	Matrix world;
	world = Matrix::Identity;			// 単位行列にする
	Renderer::SetWorldMatrix(&world);

	// ビュー変換行列セット
	Matrix view;
	view = Matrix::Identity;
	Renderer::SetViewMatrix(&view);

	Matrix projection;

	// 2D描画を左上原点にする  (20230512 update by tomoki.suzuki　
	projection = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f,
		static_cast<float>(SCREEN_WIDTH),		// ビューボリュームの最小Ｘ
		static_cast<float>(SCREEN_HEIGHT),		// ビューボリュームの最小Ｙ
		0.0f,									// ビューボリュームの最大Y
		0.0f,
		1.0f);

	Renderer::SetProjectionMatrix(&projection);

}