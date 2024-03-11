#pragma once
#include	<directxmath.h>
#include	"3DModel.h"
#include	"CBoundingSphere.h"
#include	"CShader.h"
#include	"CStaticMesh.h"
#include	"CStaticMeshRenderer.h"

struct CNormalStar {
	// プレイヤの情報(S,R,T)-----------------------------------------------------------------
	DirectX::SimpleMath::Vector3 position = { 0.0f,0.0f,0.0f };		// 位置
	DirectX::SimpleMath::Vector3 rotation = { 0.0f,0.0f,0.0f };		// 姿勢
	DirectX::SimpleMath::Vector3 scale = { 0.0f,0.0f,0.0f };		// 拡縮
	DirectX::SimpleMath::Vector3 move = { 0.0f,0.0f,0.0f };			// 移動量
	DirectX::SimpleMath::Vector3 destrot = { 0.0f,0.0f,0.0f };		// 目標回転角度
	//---------------------------------------------------------------------------------------
	// プロトタイプ宣言

	void Init();		//初期化
	void Update();		//更新
	void Draw();		//描画
	void Exit();		//メモリ解放
};
//-------------------------------------------
//情報取得
BoundingSphere GetNormalStarBS();
CNormalStar* GetNormalStar();
//-------------------------------------------