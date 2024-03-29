#pragma once
#include	<SimpleMath.h>
#include	"CShader.h"
#include	"CStaticMesh.h"
#include	"CStaticMeshRenderer.h"
#include	"sceneChange.h"
#include	"Field.h"
#include	"State.h"
#include	"BattleScene.h"
#include	"XAudio2.h"

//↓当たり判定に使用
#include	"Player.h"
#include	"weapon.h"
#include	"FireBall.h"
#include	"NormalStar.h"

using namespace DirectX::SimpleMath;
//ステータスの取得
extern ST state[];
extern ATTRIBUTE attr[];

// 球
static CSphereMesh g_sphere;
// マテリアル
static CMaterial g_material;
// 描画の為に必要な情報
// 使用するシェーダー
static CShader		g_shader;
// スタティックメッシュ（ジオメトリデータ）
static CStaticMesh g_staticmesh;
// メッシュレンダラー
static CStaticMeshRenderer g_staticmeshrenderer;
// メッシュレンダラ
static CMeshRenderer g_meshrenderer;
// Bounding sphere
static CBoundingSphere g_bs;


class CMagma {

	struct MagmaState {
		//初期ステータス
		int M_sts = 1;

		int Live = 1;	//生きている
		int Dead = 2;	//死んでいる
	};

	struct MagmaAbility {
		//基本データ
		int ID;
		const char* name;
		//ステータス
		int HP;
		int ATTACK;
		int DEFENCE;
		float SPEED;
		int DAMAGE;
		//属性
		int A_ID;
		const char* attrName;
		//属性相性
		float normal;
		float fire;
		float water;
		float ice;
		float thunder;
	};
	//---------------------
	MagmaState mgmstate;
	MagmaAbility M_abi;
	//---------------------
	Field p_field;
	//---------------------------------------
	PlayerAbility* P_abi = GetPlayerAbi();
	Player* g_player = GetPlayer();
	CFireBall* fireball = GetFireBall();
	BattleScene* BScene = GetBattleScene();
	Weapon_ST* weapon_st = GetWeaponST();
	HitStop* h_stop = GetHitStop();
	//-------------------------------------------------------------------------------------------
	const	float RATE_ROTATE_ENEMY = 0.05f;			//回転慣性係数
	const   float DISCOVERY_LENGTH = 300.0f;			//動き出す距離
	//-------------------------------------------------------------------------------------------
	float FromPlayerLength = 0.0f;		//現在のプレイヤーとの距離
	float S_Int_Leangth = 50.0f;		//プレイヤーと重ならないように(数字以下まで近寄らない)
	int DAMAGE_CHACK = 0;				//0=ヒットしない　1=ヒットする
	int ULT_HIT_INTERVAL = 0;			//必殺技のヒット間隔
	int d_HitInterval = 0;				//通常攻撃のヒット間隔（必殺技と同時に攻撃したいので）
	int NodamageTime = 1 * 60;			//ダメージ間隔（秒数＊フレーム）
	float KnockBackPower = 10.0f;		//ノックバックの距離(力)
	float KnockBackHeight = 20.0f;		//ノックバックの高さ
	float KnockBackCount = 0.0f;		//ノックバックに使用
	// プレイヤの情報(S,R,T)---------------------------------------------------------------------
	DirectX::SimpleMath::Vector3	m_position = { 0.0f,0.0f,0.0f };		// 位置
	DirectX::SimpleMath::Vector3	m_rotation = { 0.0f,0.0f,0.0f };		// 姿勢
	DirectX::SimpleMath::Vector3	m_scale = { 10.0f,10.0f,10.0f };		// 拡縮
	DirectX::SimpleMath::Vector3	m_move = { 0.0f,0.0f,0.0f };			// 移動量
	DirectX::SimpleMath::Vector3	m_destrot = { 0.0f,0.0f,0.0f };			// 目標回転角度
	DirectX::SimpleMath::Matrix		m_mtx;
	//-------------------------------------------------------------------------------------------
	
	// 描画の為に必要な情報
	// 使用するシェーダー
	CShader* m_shader = nullptr;
	// スタティックメッシュ（ジオメトリデータ）
	CStaticMesh* m_staticmesh = nullptr;
	// メッシュレンダラー
	CStaticMeshRenderer* m_staticmeshrenderer = nullptr;


public:
	//---------------------------------------------------
	//情報取得
	DirectX::SimpleMath::Vector3 GetPosition() {
		return m_position;
	}
	DirectX::SimpleMath::Vector3 GetScale() {
		return m_scale;
	}
	DirectX::SimpleMath::Vector3 GetMove() {
		return m_move;
	}
	DirectX::SimpleMath::Vector3 GetRotation() {
		return m_rotation;;
	}
	DirectX::SimpleMath::Matrix GetMatrix() {
		return m_mtx;
	}
	//---------------------------------------------------
	void SetShader(CShader* shader) {
		m_shader = shader;
	}
	void SetMesh(CStaticMesh* mesh) {
		m_staticmesh = mesh;
	}
	void SetMeshRenderer(CStaticMeshRenderer* meshrender) {
		m_staticmeshrenderer = meshrender;
	}
	//ポジションを決める
	void SetPosition(DirectX::SimpleMath::Vector3 pos) {
		m_position = pos;
	}

	//ステータス初期化-----------------------------------------------------------------------
	void InitMagma_ST() {

		mgmstate.M_sts = mgmstate.Live;
		//各値はState.cpp参照
		//基本データ
		M_abi.ID = state[2].ID;
		M_abi.name = state[M_abi.ID].name;
		//ステータス決定
		M_abi.HP = state[M_abi.ID].HP;
		M_abi.ATTACK = state[M_abi.ID].ATTACK;
		M_abi.DEFENCE = state[M_abi.ID].DEFENCE;
		M_abi.SPEED = state[M_abi.ID].SPEED;

		//属性決定
		M_abi.A_ID = attr[1].ID;
		M_abi.attrName = attr[M_abi.A_ID].attrName;
		M_abi.normal = attr[M_abi.A_ID].normal;
		M_abi.fire = attr[M_abi.A_ID].fire;
		M_abi.thunder = attr[M_abi.A_ID].thunder;
		M_abi.water = attr[M_abi.A_ID].water;
		M_abi.ice = attr[M_abi.A_ID].ice;

		//つぶれたのを直す
		m_rotation = { 0.0f,0.0f,0.0f };
		m_scale = { 10.0f,10.0f,10.0f };
	}
	//更新-----------------------------------------------------------------------------------
	void Update() {

		// 高さを取得
		float height;
		height = GetFieldHeight(m_position);

		// プレイヤの座標を取得
		DirectX::SimpleMath::Vector3 playerpos = GetPositionPlayer();

		//プレイヤーとの距離を取得
		FromPlayerLength = (playerpos - m_position).Length();

		//敵が倒れている時-------------------------------------------------------------------------
		if (mgmstate.M_sts == mgmstate.Dead) {
			DeadUpdate();
			//ぺっちゃんこにする
			m_scale.y = 3.0f;
			m_scale.x = m_scale.z = 12.0f;
			m_rotation.x = m_rotation.z = 0.0f;
			m_position.y = height + m_scale.y;
			//ノックバック
			if (DAMAGE_CHACK == 1) {
				KnockBackCount += (1.0f / NodamageTime * 3.0f);		//sinカーブを使用　1/180
				m_position -= (m_move / FromPlayerLength) * KnockBackPower / 2;
				m_position.y = (height+m_scale.y) + sinf(KnockBackCount) * KnockBackHeight;
			}
			else {
				m_move = playerpos - m_position;		//移動
				KnockBackCount = 0;
			}
		}

		// 範囲チェック----------------------------------------------------------------------------

		if (m_position.x <= -p_field.FieldX_MAX / 2.5f) {
			m_position.x = -p_field.FieldX_MAX / 2.5f;
		}
		if (m_position.x >= p_field.FieldX_MAX / 2.5f) {
			m_position.x = p_field.FieldX_MAX / 2.5f;
		}
		if (m_position.z >= p_field.FieldZ_MAX / 2.5f) {
			m_position.z = p_field.FieldZ_MAX / 2.5f;
		}
		if (m_position.z <= -p_field.FieldZ_MAX / 2.5f) {
			m_position.z = -p_field.FieldZ_MAX / 2.5f;
		}

		//----------------------------------------------------------------------------------------
		//敵が倒れていると処理しない
		if (mgmstate.M_sts != mgmstate.Live)return;

		Battle();

		if (g_player->ULT_I == 1)return;		//氷のスキルを発動したら動かなくする(凍る)

		//ダメージ時ノックバック
		//ノックバック方向を固定するために、ダメージ時にm_moveを更新しない
		if (DAMAGE_CHACK == 1) {
			KnockBackCount += (1.0f / NodamageTime * 3.0f);		//sinカーブを使用　1/180
			m_position -= (m_move / FromPlayerLength) * KnockBackPower;
			m_position.y = (height + m_scale.y) + sinf(KnockBackCount) * KnockBackHeight;
		}
		else {
			m_position.y = height + m_scale.y;		//高さ
			m_move = playerpos - m_position;		//移動
			KnockBackCount = 0;
		}

		//一定距離離れていると処理しない
		if (FromPlayerLength >= DISCOVERY_LENGTH)return;

		// atan2を使用して角度を求める
		m_destrot.y = atan2f(-(playerpos.x - m_position.x), -(playerpos.z - m_position.z));

		// 現在の向きとの差分を計算する
		float fDiffRotY = m_destrot.y - m_rotation.y;

		//---------------------------------------------------------------------------------------------
		// 移動処理
		//HPが一定以下で逃げる
		if (M_abi.HP < state[M_abi.ID].HP / 2) {

			m_position -= (m_move / FromPlayerLength) * M_abi.SPEED;

			// 補正（−１８０〜１８０の範囲）
			if (fDiffRotY > DirectX::XM_PI) {
				fDiffRotY -= DirectX::XM_PI * 2.0f;
			}
			if (fDiffRotY < -DirectX::XM_PI) {
				fDiffRotY += DirectX::XM_PI * 2.0f;
			}

			// 回転角度計算
			m_rotation.y -= fDiffRotY * RATE_ROTATE_ENEMY;
			if (m_rotation.y > DirectX::XM_PI) {
				m_rotation.y -= DirectX::XM_PI * 2.0f;
			}
			if (m_rotation.y < -DirectX::XM_PI) {
				m_rotation.y += DirectX::XM_PI * 2.0f;
			}
			m_rotation.x -= 0.1f;
		}
		else {
			//HPが一定以上なら近づく
			if (FromPlayerLength <= S_Int_Leangth)return;
			m_position += (m_move / FromPlayerLength) * M_abi.SPEED;

			// 補正（−１８０〜１８０の範囲）
			if (fDiffRotY > DirectX::XM_PI) {
				fDiffRotY -= DirectX::XM_PI * 2.0f;
			}
			if (fDiffRotY < -DirectX::XM_PI) {
				fDiffRotY += DirectX::XM_PI * 2.0f;
			}

			// 回転角度計算
			m_rotation.y += fDiffRotY * RATE_ROTATE_ENEMY;
			if (m_rotation.y > DirectX::XM_PI) {
				m_rotation.y -= DirectX::XM_PI * 2.0f;
			}
			if (m_rotation.y < -DirectX::XM_PI) {
				m_rotation.y += DirectX::XM_PI * 2.0f;
			}
			m_rotation.x -= 0.1f;
		}
	}
	//バトルの当たり判定処理-----------------------------------------------------------------
	void Battle() {

		// プレイヤの攻撃に使用するBSを取得
		BoundingSphere pbs = GetWeaponBS();
		BoundingSphere fbs = GetFireBallBS();
		BoundingSphere sbs = GetNormalStarBS();

		Matrix Mmtx;
		Mmtx = GetMatrix();							// 敵の行列を取得
		BoundingSphere mbs;

		mbs = g_bs.MakeBS(Mmtx, GetScale());

		//プレイヤーの攻撃ヒット判定
		bool sts = HitCheckSphere(pbs, mbs);
		//当っている&&ダメージ間隔がとれている&&武器の当たり判定が生きている
		if (sts && DAMAGE_CHACK == 0 && weapon_st->WeaponHitCheck == 1) {
			DamageCalc();
			DAMAGE_CHACK = 1;
			h_stop->hitchack = true;
			PlaySound(SE5);
		}
		//ダメージ間隔
		if (DAMAGE_CHACK == 1) {
			d_HitInterval++;
			if (d_HitInterval >= NodamageTime) {
				DAMAGE_CHACK = 0;
				d_HitInterval = 0;
			}
		}

		//火の必殺技に対する処理
		bool usts = HitCheckSphere(fbs, mbs);
		if (g_player->ULT_F == 1) {
			ULT_HIT_INTERVAL++;
			//ヒット間隔処理
			if (usts && ULT_HIT_INTERVAL >= NodamageTime) {
				ULTDamageCalc();
				h_stop->hitchack = true;
				ULT_HIT_INTERVAL = 0;
			}
		}
		//無属性の必殺技に対する処理
		bool nssts = HitCheckSphere(sbs, mbs);
		if (g_player->ULT_N == 1) {
			M_abi.DEFENCE /= 10;						//当たっていると防御力減
		}
		else{
			M_abi.DEFENCE = state[M_abi.ID].DEFENCE;	//当っていないと初期値を入れる
		}

		//雷属性の必殺技に対する処理（仮）
		if (g_player->ULT_T == 1 && DAMAGE_CHACK == 0
			&& FromPlayerLength <= DISCOVERY_LENGTH) {
			DAMAGE_CHACK = 1;
			ULTDamageCalc();
			h_stop->hitchack = true;
		}

		//敵を倒す処理
		if (M_abi.HP <= 0) {
			mgmstate.M_sts = mgmstate.Dead;
			//シーンチェンジに使う変数
			BScene->SceneChangeCnt += 1;		
		}
	}

	//プレイヤーの通常攻撃の計算-------------------------------------------------------------
	void DamageCalc() {
		//プレイヤーの攻撃力とエネミーの防御力からダメージを計算
		P_abi->DAMAGE = P_abi->ATTACK - M_abi.DEFENCE;
		//プレイヤーの属性と応じた属性耐性の倍率をダメージに掛ける
		switch (P_abi->A_ID) {
		case 0:
			P_abi->DAMAGE = P_abi->DAMAGE * M_abi.normal;
			break;
		case 1:
			P_abi->DAMAGE = P_abi->DAMAGE * M_abi.fire;
			break;
		case 2:
			P_abi->DAMAGE = P_abi->DAMAGE * M_abi.water;
			break;
		case 3:
			P_abi->DAMAGE = P_abi->DAMAGE * M_abi.ice;
			break;
		case 4:
			P_abi->DAMAGE = P_abi->DAMAGE * M_abi.thunder;
			break;
		default:
			break;
		}

		//チャージ出来ていたらダメージに倍率
		if (weapon_st->Max_charge == true) {
			P_abi->DAMAGE *= weapon_st->chargeMag;
		}

		//ダメージを与える
		M_abi.HP = M_abi.HP - P_abi->DAMAGE;
	}

	//プレイヤーの必殺技の計算---------------------------------------------------------------
	void ULTDamageCalc() {
		P_abi->DAMAGE = P_abi->ATTACK - M_abi.DEFENCE;

		//属性毎に倍率変更
		switch (P_abi->A_ID) {
		case 0:
			P_abi->DAMAGE = P_abi->DAMAGE * M_abi.normal;
			break;
		case 1:
			P_abi->DAMAGE = P_abi->DAMAGE * M_abi.fire;
			break;
		case 2:
			P_abi->DAMAGE = P_abi->DAMAGE * M_abi.water;
			break;
		case 3:
			P_abi->DAMAGE = P_abi->DAMAGE * M_abi.ice;
			break;
		case 4:
			P_abi->DAMAGE = P_abi->DAMAGE * M_abi.thunder;
			break;
		default:
			break;
		}
		P_abi->DAMAGE *= 2;
		M_abi.HP = M_abi.HP - P_abi->DAMAGE;
	}

	//死んでる時の動き（遊びでいれた）-------------------------------------------------------
	void DeadUpdate() {
		// プレイヤーの武器BSを取得
		BoundingSphere pwbs = GetWeaponBS();
		// プレイヤーのBSを取得
		BoundingSphere pbs = GetPlayerBS();

		Matrix smtx;
		smtx = GetMatrix();							// 敵の行列を取得
		BoundingSphere sbs;
		sbs = g_bs.MakeBS(smtx, GetScale());

		//プレイヤーの攻撃ヒット判定
		bool sts = HitCheckSphere(pwbs, sbs);
		//当っている&&ダメージ間隔がとれている&&武器の当たり判定が生きている
		if (sts && DAMAGE_CHACK == 0 && weapon_st->WeaponHitCheck == 1) {
			DAMAGE_CHACK = 1;
			PlaySound(SE6);
		}
		if (DAMAGE_CHACK == 1) {
			d_HitInterval += 1;
			if (d_HitInterval >= NodamageTime) {
				DAMAGE_CHACK = 0;
				d_HitInterval = 0;
			}
		}

		//プレイヤーとの当たり判定
		bool pts = HitCheckSphere(pbs, sbs);
		if (pts) {
			m_position -= (m_move / FromPlayerLength) * M_abi.SPEED;
		}
	}

	// 描画----------------------------------------------------------------------------------
	void Draw() {

		//一定距離内のみ表示
		if (FromPlayerLength >= 700)return;
		// デバイスコンテキスト取得
		ID3D11DeviceContext* devicecontext;
		devicecontext = Renderer::GetDeviceContext();

		// ワールド変換行列生成
		DirectX::SimpleMath::Matrix mtx;
		DX11MakeWorldMatrixRadian(
			mtx,
			m_scale,							// スケール
			m_rotation,							// 姿勢
			m_position);						// 位置

		m_mtx = mtx;

		// GPUに行列をセットする
		Renderer::SetWorldMatrix(&mtx);

		// シェーダーをGPUにセット
		m_shader->SetGPU();

		// モデル描画
		m_staticmeshrenderer->Draw();
		
	}
};