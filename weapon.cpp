#include	"weapon.h"
#include	"Player.h"
#include	"XAudio2.h"

using namespace DirectX::SimpleMath;

// プレイヤの情報(S,R,T)-------------------------------------------------------
static Vector3	g_position = { 0.0f,0.0f,0.0f };		// 位置
static Vector3	g_rotation = { 0.0f,0.0f,0.0f };		// 姿勢
static Vector3	g_scale = { 2.0f,2.0f,2.0f };			// 拡縮
static Vector3	g_move = { 0.0f,0.0f,0.0f };			// 移動量
static Vector3	g_destrot = { 0.0f,0.0f,0.0f };			// 目標回転角度
DirectX::XMFLOAT3 wTrans(0, 0, 0);
//-----------------------------------------------------------------------------
// Bounding sphere
static CBoundingSphere g_bs;
// 球
static CSphereMesh g_sphere;
// メッシュレンダラ
static CMeshRenderer g_meshrenderer;
// マテリアル
static CMaterial g_material;

// 描画の為に必要な情報
// 使用するシェーダー
static CShader		g_shader;

// スタティックメッシュ（ジオメトリデータ）
static CStaticMesh g_staticmesh;

// メッシュレンダラー
static CStaticMeshRenderer g_staticmeshrenderer;

Weapon_ST weapon_st;
HitStop h_stop;
//--------------------------------------------------------
//情報取得
Weapon_ST* GetWeaponST() {
	return &weapon_st;
}

HitStop* GetHitStop() {
	return &h_stop;
}

//位置取得
DirectX::XMFLOAT3 GetPositionWeapon() {
	return g_position;
}

BoundingSphere GetWeaponBS() {

	// ワールド変換行列生成
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_scale,							// スケール
		g_rotation,							// 姿勢
		g_position);						// 位置

	// BS生成
	BoundingSphere bs;
	bs = g_bs.MakeBS(mtx, g_scale);

	return bs;
}
//--------------------------------------------------------

//初期化
void InitWeapon()
{
	// 使用するシェーダーを生成
	g_shader.SetShader(
		"shader/vertexLightingVS.hlsl",					// 頂点シェーダ
		"shader/vertexLightingPS.hlsl");				// ピクセルシェーダ

	// モデルファイル名
	std::string filename[] = {
		u8"assets\\model\\sowrd.obj",
	};

	// メッシュ生成（ジオメトリデータ)
	g_staticmesh.Init(filename[0]);

	// 描画の為のデータ生成
	g_staticmeshrenderer.Init(g_staticmesh);

	// 境界球を求める
	g_bs.Caliculate(g_staticmesh.GetVertices());

	g_sphere.Init(g_bs.GetRadius(), Color(1, 1, 1, 1), 100, 100);
	g_meshrenderer.Init(g_sphere);

	MATERIAL mtrl;
	mtrl.Ambient = Color(0, 0, 0, 0);
	mtrl.Diffuse = Color(1, 1, 1, 0.3f);
	mtrl.Specular = Color(0, 0, 0, 0);
	mtrl.Shininess = 0;
	mtrl.Emission = Color(0, 0, 0, 0);
	mtrl.TextureEnable = FALSE;

	g_material.Init(mtrl);

}
//メモリ解放
void ExitWeapon() {
	g_staticmeshrenderer.Uninit();
	g_material.Uninit();
}
//更新
void UpdateWeapon() {

	g_position = GetPositionPlayer();

	g_rotation = GetRotationPlayer();				// プレイヤの向き取得

	g_move = GetMovePlayer();				// プレイヤの移動量取得

	Matrix mtx;
	DX11MakeWorldMatrixRadian(mtx, g_scale, g_rotation, wTrans);

	Vector3 zAxis;
	zAxis.x = mtx._31;
	zAxis.y = mtx._32;
	zAxis.z = mtx._33;
	
	g_position = g_position + zAxis * 4.0f;
	g_position.y = g_position.y + 5.0f;

	Vector3 rAxis;
	rAxis.x = mtx._21;
	rAxis.y = mtx._22;
	rAxis.z = mtx._23;

	g_rotation = g_rotation + rAxis;
	
	//攻撃アニメーション--------------------------------------------------------

	if (CDirectInput::GetInstance().GetMouseRButtonCheck() &&
		weapon_st.WeaponHitCheck != 1) {
		//チャージ変数
		if (weapon_st.charge <= weapon_st.chargeTime) {
			weapon_st.charge++;
		}
		//チャージ時の位置
		g_position = g_position - zAxis * 4.0f;
		g_position.y = g_position.y + 7.0f;
		g_rotation.x = 300.0f;

		if (CDirectInput::GetInstance().GetMouseLButtonTrigger() ||
			CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_E) &&
			weapon_st.charge >= weapon_st.chargeTime
			&& weapon_st.attTime == 0) {

			weapon_st.Max_charge = true;
			weapon_st.WeaponHitCheck = 1;
			PlaySound(SE1);
		}
	}
	else{
		if (CDirectInput::GetInstance().GetMouseLButtonTrigger() ||
			CDirectInput::GetInstance().CheckKeyBufferTrigger(DIK_E) &&
			weapon_st.attTime == 0) {
			weapon_st.WeaponHitCheck = 1;
			PlaySound(SE1);
		}
		weapon_st.charge = 0.0f;
	}

	if (weapon_st.WeaponHitCheck == 1) {
		g_position = g_position - zAxis * 4.0f;
		g_position.y = g_position.y + 10.0f;

		g_rotation.x -= weapon_st.attTime * 0.1f;
		g_rotation.y -= weapon_st.attTime * 0.1f;
		
		weapon_st.attTime++;
	}
	//一定時間経過で終了
	if (weapon_st.attTime >= weapon_st.attackCoolTime) {
		weapon_st.attTime = 0;
		weapon_st.WeaponHitCheck = 0;
		weapon_st.charge = 0.0f;
		weapon_st.Max_charge = false;
	}
	//--------------------------------------------------------------------------
}
//描画
void DrawWeapon()
{
	// デバイスコンテキスト取得
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// ワールド変換行列生成
	Matrix mtx;
	DX11MakeWorldMatrixRadian(
		mtx,
		g_scale,							// スケール
		g_rotation,							// 姿勢
		g_position);						// 位置

	// GPUに行列をセットする
	Renderer::SetWorldMatrix(&mtx);

	// シェーダーをGPUにセット
	g_shader.SetGPU();

	// モデル描画
	g_staticmeshrenderer.Draw();

}