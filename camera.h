#pragma once

#include	<SimpleMath.h>

class Camera {
private:
	//カメラの情報-----------------------------------------------------------

	static DirectX::SimpleMath::Vector3	m_Position;
	static DirectX::SimpleMath::Vector3	m_Rotation;
	static DirectX::SimpleMath::Vector3	m_Scale;

	//カメラに使用する変数---------------------------------------------------

	static DirectX::SimpleMath::Vector3	m_Target;		//注視点
	static DirectX::SimpleMath::Matrix	m_ViewMatrix;	//ビューマトリクス

	static float m_TargetLength;						//注視点との距離

	static DirectX::SimpleMath::Vector3 m_TargetDest;	//注視点の目標位置
	static DirectX::SimpleMath::Vector3 m_PositionDest;	//視点の目標位置

	static float m_HeightCameraP;						//追尾する時の位置（高さ）
	static float m_HeightCameraR;						//追尾する時の注視点の位置（高さ）

	//-----------------------------------------------------------------------

public:

	//プロトタイプ宣言-------------------------------------------------------

	//初期化
	static void Init();
	//バトルシーン用カメラ
	static void UpdateBS();
	//ボスシーン用カメラ
	static void UpdateBossS();
	//チャージ中のカメラアニメーション
	static void ChargeAnime();
	//カメラを揺らす
	static void ShakeCamera();
	//描画用
	static void Draw();
	//2D描画用
	static void Draw2D();

	//情報取得--------------------------------------------------------------

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