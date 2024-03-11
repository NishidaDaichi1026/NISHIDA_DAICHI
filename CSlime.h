#pragma once
#include	<SimpleMath.h>
#include	"CShader.h"
#include	"CStaticMesh.h"
#include	"CStaticMeshRenderer.h"
#include	"State.h"
#include	"Field.h"
#include	"BattleScene.h"
#include	"XAudio2.h"

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
DirectX::XMFLOAT3 sTrans(0.0f, 0.0f, 0.0f);

class CSLIME {

	struct SlimeState {
		//初期ステータス
		int S_sts = 1;

		int Live = 1;	//生きている
		int Dead = 2;	//死んでいる
	};

	struct SlimeAbility {
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
	//--------------------------------
	SlimeState slimestate;
	SlimeAbility S_abi;
	//--------------------------------
	Field p_field;

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
	float KnockBackPower = 15.0f;		//ノックバックの距離(力)
	float KnockBackHeight = 30.0f;		//ノックバックの高さ
	float KnockBackCount = 0.0f;		//ノックバックに使用
	
	// プレイヤの情報(S,R,T)------------------------------------------------------------------------
	DirectX::SimpleMath::Vector3	m_position = { 0.0f,0.0f,0.0f };		// 位置
	DirectX::SimpleMath::Vector3	m_rotation = { 0.0f,0.0f,0.0f };		// 姿勢
	DirectX::SimpleMath::Vector3	m_scale = { 1.5f,1.5f,1.5f };			// 拡縮
	DirectX::SimpleMath::Vector3	m_move = { 0.0f,0.0f,0.0f };			// 移動量
	DirectX::SimpleMath::Vector3	m_destrot = { 0.0f,0.0f,0.0f };			// 目標回転角度
	DirectX::SimpleMath::Matrix		m_mtx;
	
	DirectX::SimpleMath::Vector3	m_holdposition = { 0.0f,0.0f,0.0f };		// 位置
	//----------------------------------------------------------------------------------------------

	// 描画の為に必要な情報
	// 使用するシェーダー
	CShader* m_shader = nullptr;
	// スタティックメッシュ（ジオメトリデータ）
	CStaticMesh* m_staticmesh = nullptr;
	// メッシュレンダラー
	CStaticMeshRenderer* m_staticmeshrenderer = nullptr;


public:
	//--------------------------------------------------------------
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
	//--------------------------------------------------------------
	//外部からセット
	void SetShader(CShader* shader) {
		m_shader = shader;
	}
	void SetMesh(CStaticMesh* mesh) {
		m_staticmesh = mesh;
	}
	void SetMeshRenderer(CStaticMeshRenderer* meshrender) {
		m_staticmeshrenderer = meshrender;
	}
	void SetPosition(DirectX::SimpleMath::Vector3 pos) {
		m_position = pos;
	}
	//--------------------------------------------------------------

	//ステータス初期化-----------------------------------------------------------------------
	void InitSlime_ST() {

		slimestate.S_sts = slimestate.Live;
		//各値はState.cpp参照
		//基本データ
		S_abi.ID = state[1].ID;
		S_abi.name = state[S_abi.ID].name;
		//ステータス決定
		S_abi.HP = state[S_abi.ID].HP;
		S_abi.ATTACK = state[S_abi.ID].ATTACK;
		S_abi.DEFENCE = state[S_abi.ID].DEFENCE;
		S_abi.SPEED = state[S_abi.ID].SPEED;

		//属性決定
		S_abi.A_ID = attr[3].ID;
		S_abi.attrName = attr[S_abi.A_ID].attrName;
		S_abi.normal = attr[S_abi.A_ID].normal;
		S_abi.fire = attr[S_abi.A_ID].fire;
		S_abi.thunder = attr[S_abi.A_ID].thunder;
		S_abi.water = attr[S_abi.A_ID].water;
		S_abi.ice = attr[S_abi.A_ID].ice;

		//つぶれたのを直す
		m_rotation = { 0.0f,0.0f,0.0f };
		m_scale = { 1.5f,1.5f,1.5f };
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

		//敵が倒れている時
		if (slimestate.S_sts == slimestate.Dead) {
			DeadUpdate();
			//ノックバック
			if (DAMAGE_CHACK == 1) {
				KnockBackCount += (1.0f / NodamageTime * 3.0f);		//sinカーブを使用　1/180
				m_position -= (m_move / FromPlayerLength) * KnockBackPower / 2;
				m_position.y = (height * 2) + sinf(KnockBackCount) * KnockBackHeight;
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
		if (slimestate.S_sts != slimestate.Live)return;

		Battle();

		if (g_player->ULT_I == 1)return;		//氷のスキルを発動したら動かなくする(凍る)

		//ダメージ時ノックバック
		//ノックバック方向を固定するために、ダメージ時にm_moveを更新しない
		if (DAMAGE_CHACK == 1) {
			KnockBackCount += (1.0f / NodamageTime * 3.0f);		//sinカーブを使用　1/180
			m_position -= (m_move / FromPlayerLength) * KnockBackPower;
			m_position.y = height + sinf(KnockBackCount) * KnockBackHeight;
		}
		else {
			m_position.y = height;					//高さ
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
		if (S_abi.HP < state[S_abi.ID].HP / 2) {

			m_position -= (m_move / FromPlayerLength) * S_abi.SPEED;

			// 補正（－１８０～１８０の範囲）
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
		}
		else {
			//HPが一定以上なら近づく
			if (FromPlayerLength <= S_Int_Leangth)return;
			m_position += (m_move / FromPlayerLength) * S_abi.SPEED;

			// 補正（－１８０～１８０の範囲）
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
		}
		//---------------------------------------------------------------------------------------------
	}

	//バトルの処理---------------------------------------------------------------------------
	void Battle(){
		// プレイヤの武器BSを取得
		BoundingSphere pbs = GetWeaponBS();			// 中心座標のワールド変換行列を取得
		BoundingSphere fbs = GetFireBallBS();
		BoundingSphere nsbs = GetNormalStarBS();

		Matrix smtx;
		smtx = GetMatrix();							// 敵の行列を取得
		BoundingSphere sbs;

		sbs = g_bs.MakeBS(smtx, GetScale());

		//プレイヤーの攻撃ヒット判定
		bool sts = HitCheckSphere(pbs, sbs);
		//当っている&&ダメージ間隔がとれている&&武器の当たり判定が生きている
		if (sts && DAMAGE_CHACK == 0 && weapon_st->WeaponHitCheck == 1) {
			DamageCalc();
			DAMAGE_CHACK = 1;
			h_stop->hitchack = true;
			PlaySound(SE6);
		}
		if (DAMAGE_CHACK == 1) {
			d_HitInterval += 1;
			if (d_HitInterval >= NodamageTime) {
				DAMAGE_CHACK = 0;
				d_HitInterval = 0;
			}
		}

		//火の必殺技に対する処理
		bool usts = HitCheckSphere(fbs, sbs);
		if (g_player->ULT_F == 1) {
			ULT_HIT_INTERVAL++;
			if (usts && ULT_HIT_INTERVAL >= NodamageTime) {
				ULTDamageCalc();
				h_stop->hitchack = true;
				ULT_HIT_INTERVAL = 0;
			}
		}

		//無属性の必殺技に対する処理
		bool nssts = HitCheckSphere(nsbs, sbs);
		if (g_player->ULT_N == 1) {
			S_abi.DEFENCE /= 10;						//当たっていると防御力減
		}
		else {
			S_abi.DEFENCE = state[S_abi.ID].DEFENCE;	//当っていないと初期値を入れる
		}

		//雷属性の必殺技に対する処理（仮）
		if (g_player->ULT_T == 1 && DAMAGE_CHACK == 0
			&& FromPlayerLength <= DISCOVERY_LENGTH) {
			DAMAGE_CHACK = 1;
			ULTDamageCalc();
			h_stop->hitchack = true;
		}

		//敵を倒す処理
		if (S_abi.HP <= 0) {
			slimestate.S_sts = slimestate.Dead;
			BScene->SceneChangeCnt += 1;
		}
	}

	//プレイヤーの通常攻撃の計算-------------------------------------------------------------
	void DamageCalc() {
		P_abi->DAMAGE = P_abi->ATTACK - S_abi.DEFENCE;

		//属性毎に倍率変更
		switch (P_abi->A_ID) {
		case 0:
			P_abi->DAMAGE = P_abi->DAMAGE * S_abi.normal;
			break;
		case 1:
			P_abi->DAMAGE = P_abi->DAMAGE * S_abi.fire;
			break;
		case 2:
			P_abi->DAMAGE = P_abi->DAMAGE * S_abi.water;
			break;
		case 3:
			P_abi->DAMAGE = P_abi->DAMAGE * S_abi.ice;
			break;
		case 4:
			P_abi->DAMAGE = P_abi->DAMAGE * S_abi.thunder;
			break;
		default:
			break;
		}

		//チャージ出来ていたらダメージに倍率
		if (weapon_st->Max_charge == true) {
			P_abi->DAMAGE *= weapon_st->chargeMag;
		}

		S_abi.HP = S_abi.HP - P_abi->DAMAGE;
	}

	//プレイヤーの必殺技の計算---------------------------------------------------------------
	void ULTDamageCalc() {
		P_abi->DAMAGE = P_abi->ATTACK - S_abi.DEFENCE;

		//属性毎に倍率変更
		switch (P_abi->A_ID) {
		case 0:
			P_abi->DAMAGE = P_abi->DAMAGE * S_abi.normal;
			break;
		case 1:
			P_abi->DAMAGE = P_abi->DAMAGE * S_abi.fire;
			break;
		case 2:
			P_abi->DAMAGE = P_abi->DAMAGE * S_abi.water;
			break;
		case 3:
			P_abi->DAMAGE = P_abi->DAMAGE * S_abi.ice;
			break;
		case 4:
			P_abi->DAMAGE = P_abi->DAMAGE * S_abi.thunder;
			break;
		default:
			break;
		}
		P_abi->DAMAGE *= 2;
		S_abi.HP = S_abi.HP - P_abi->DAMAGE;
	}

	//死んでる時の動き-----------------------------------------------------------------------
	void DeadUpdate() {

		// 高さを取得
		float height;
		height = GetFieldHeight(m_position);
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
			//隠しコマンド（持ち歩ける）
			if (CDirectInput::GetInstance().CheckKeyBuffer(DIK_C)) {
				m_position = GetPositionPlayer();
				m_rotation = GetRotationPlayer();

				Matrix mtx;
				DX11MakeWorldMatrixRadian(mtx, m_scale, m_rotation, sTrans);

				Vector3 zAxis;
				zAxis.x = mtx._31;
				zAxis.y = mtx._32;
				zAxis.z = mtx._33;

				m_position = m_position - zAxis * 5.0f;
				m_position.y = m_position.y + 5.0f;
			}
			else {
				m_position -= (m_move / FromPlayerLength) * S_abi.SPEED;
				m_position.y = height * 2;
				//ひっくり返す
				m_rotation.z = 90.0f;
			}
		}
		else{
			m_position.y = height * 2;
			//ひっくり返す
			m_rotation.z = 90.0f;
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