#include <DxLib.h>
#include <string>
#include <memory>
#include "../../Application.h"
#include "../../Utility/AsoUtility.h"
#include "../../Manager/InputManager.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Camera.h"
#include "../Common/AnimationController.h"
#include "../Common/Capsule.h"
#include "../Common/Collider.h"
#include "Planet.h"
#include "Player.h"
#include "../../Manager/EffectManager.h"
#include "../Effect/WindEffect.h"
#include "Enemy.h"

Enemy::Enemy(void)
{
	speed_ = 0.0f;
	moveDir_ = AsoUtility::VECTOR_ZERO;
	movePow_ = AsoUtility::VECTOR_ZERO;
	movedPos_ = AsoUtility::VECTOR_ZERO;

	playerRotY_ = Quaternion();
	goalQuaRot_ = Quaternion();
	stepRotTime_ = 0.0f;

	// 衝突チェック
	gravHitPosDown_ = AsoUtility::VECTOR_ZERO;
	gravHitPosUp_ = AsoUtility::VECTOR_ZERO;

	hitColorTimer_ = 0.0f;
	isHit_ = false;

	capsule_ = nullptr;

}

Enemy::~Enemy(void)
{
}

void Enemy::Init(void)
{
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::ENEMY));
	transform_.scl = { 0.3f, 0.3f, 0.3f };
	transform_.pos = { 0.0f, -30.0f, 1000.0f };
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_.Update();

	InitAnimation();

	// カプセルコライダ
	capsule_ = std::make_unique<Capsule>(transform_);
	capsule_->SetLocalPosTop({ 0.0f, 110.0f, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	capsule_->SetRadius(20.0f);

	targetPos_ = transform_.pos;
	targetPos_.y = -80.0f;
	targetTimer_ = 0.0f;

	hp_ = 100;

	findDistance_ = FIND_DISTANCE;
	chargeTimer_ = 0.0f;

	state_ = STATE::NONE;
	
	attackCoolTime_ = 3.0f;

	deadTimer_ = 5.0f;

}

void Enemy::Update(void)
{
	if (isHit_)
	{
		hitColorTimer_ -= scnMng_.GetDeltaTime();
		if (hitColorTimer_ <= 0.0f)
		{
			isHit_ = false;
		}
	}

	if (IsDead())
	{
		deadTimer_ -= scnMng_.GetDeltaTime();

		if (deadTimer_ <= 0.0f)
		{
			Respawn();
		}
		return;
	}
	UpdatePlay();

	// アニメーション再生
	animationController_->Update();

	transform_.Update();
}

void Enemy::Draw(void)
{
	if (IsDead())
	{
		return;
	}

	bool flash = ((int)(hitColorTimer_ * 5) % 2) == 0;

	if (isHit_ && flash)
	{
		MV1SetDifColorScale(
			transform_.modelId,
			GetColorF(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		MV1SetDifColorScale(
			transform_.modelId,
			GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	}

	DrawShadow();
	MV1DrawModel(transform_.modelId);
}

void Enemy::Damage(int damage)
{
	hp_ -= damage;

	if (hp_ < 0)
	{
		hp_ = 0;
	}

	isHit_ = true;
	hitColorTimer_ = 0.2f;
}

bool Enemy::IsDead() const
{
	return hp_ <= 0;
}

void Enemy::SetPlayer(Player* player)
{
	player_ = player;
}

void Enemy::Respawn(void)
{
	deadTimer_ = 5.0f;

	// HP回復
	hp_ = 100;

	// 空中ランダム位置
	transform_.pos.x = (float)(rand() % 2000 - 1000);
	transform_.pos.y = (float)(rand() % 500 + 200);  // 高さ200～700
	transform_.pos.z = (float)(rand() % 2000 - 1000);

	// 回転リセット
	playerRotY_ = Quaternion();
	goalQuaRot_ = Quaternion();

	// 移動停止
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 徘徊状態へ
	state_ = STATE::NONE;

	// 攻撃待機時間
	attackCoolTime_ = 3.0f;

	// アニメーション再開
	animationController_->Play(
		(int)ANIM_TYPE::IDLE,
		true,
		0.0f,
		0.0f,
		false,
		true
	);

	transform_.Update();
	targetPos_ = transform_.pos;
	targetTimer_ = 0.0f;
}

void Enemy::SetEffectManager(EffectManager* effectMng)
{
	effectMng_ = effectMng;
}

void Enemy::AddCollider(Collider* collider)
{
	colliders_.push_back(collider);
}

void Enemy::ClearCollider(void)
{
	colliders_.clear();
}

const Capsule* Enemy::GetCapsule(void) const
{
	return capsule_.get();
}

void Enemy::InitAnimation(void)
{

	std::string path = Application::PATH_MODEL + "Enemy/";
	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	animationController_->Add((int)ANIM_TYPE::IDLE, path + "enemy_anim.mv1", 25.0f);
	
	animationController_->Play((int)ANIM_TYPE::IDLE, true);
}

void Enemy::UpdateNone(void)
{
}

void Enemy::UpdatePlay(void)
{
	if (attackCoolTime_ > 0.0f)
	{
		attackCoolTime_ -= scnMng_.GetDeltaTime();
	}

	switch (state_)
	{
	case STATE::NONE:
	{
		ProcessMove();

		VECTOR dir = VSub(player_->GetPos(), transform_.pos);

		if (VSize(dir) < FIND_DISTANCE &&
			attackCoolTime_ <= 0.0f)
		{
			state_ = STATE::CHASE;
		}
		break;
	}

	case STATE::CHASE:
	{
		VECTOR toPlayer =
			VSub(player_->GetPos(), transform_.pos);

		float dist = VSize(toPlayer);

		VECTOR dir = VNorm(toPlayer);

		// ゆっくり追う
		speed_ = 2.0f;
		movePow_ = VScale(dir, speed_);

		float rotY = atan2f(dir.x, dir.z);
		SetGoalRotate(rotY);

		// 近すぎたら攻撃へ
		if (dist < ATTACK_DISTANCE)
		{
			state_ = STATE::ATTACK;

			// 攻撃方向固定（ここ重要：追尾暴れ防止）
			moveDir_ = VNorm(VSub(player_->GetPos(), transform_.pos));

			chargeTimer_ = 1.0f; // 少し溜めるとそれっぽい
		}

		// 遠くなったら戻る
		if (dist > FIND_DISTANCE * 1.5f)
		{
			state_ = STATE::NONE;
		}

		break;
	}

	case STATE::ATTACK:
	{
		speed_ = 10.0f;
		movePow_ = VScale(moveDir_, speed_);

		// 突進中の風
		windTimer_ -= scnMng_.GetDeltaTime();

		if (windTimer_ <= 0.0f)
		{
			windTimer_ = 0.05f;

			float angle = (float)(rand() % 360) * DX_PI_F / 180.0f;
			float radius = 50.0f + rand() % 80;

			VECTOR offset = VGet(
				cosf(angle) * radius,
				(float)(rand() % 100),
				sinf(angle) * radius
			);

			VECTOR windPos =
				VAdd(transform_.pos, offset);

			// 後ろに流れる風
			VECTOR windDir =
				VScale(moveDir_, -1.0f);

			effectMng_->Add(
				std::make_unique<WindEffect>(
					windPos,
					windDir));
		}

		chargeTimer_ -= scnMng_.GetDeltaTime();

		if (chargeTimer_ <= 0.0f)
		{
			state_ = STATE::RECOVER;
			chargeTimer_ = 1.0f;

			attackCoolTime_ = 3.0f;
		}
		// 衝突判定
		const Capsule* playerCap = player_->GetCapsule();

		if (HitCheck_Capsule_Capsule(
			capsule_->GetPosTop(),
			capsule_->GetPosDown(),
			capsule_->GetRadius(),
			playerCap->GetPosTop(),
			playerCap->GetPosDown(),
			playerCap->GetRadius()))
		{
			player_->Damage(30);

			state_ = STATE::RECOVER;
		}

		break;
	}

	case STATE::RECOVER:
	{
		VECTOR away = VGet(0.0f, 1.0f, 0.0f);

		speed_ = 10.0f;
		movePow_ = VScale(away, speed_);

		float dist = transform_.pos.y - player_->GetPos().y;

		if (dist >= RECOVER_DISTANCE)
		{
			state_ = STATE::NONE;
		}

		break;
	}

	}
	// 移動方向に応じた回転
	Rotate();

	transform_.pos = VAdd(transform_.pos, movePow_);

	// 回転させる
	transform_.quaRot = playerRotY_;

}

void Enemy::DrawShadow(void)
{

	float PLAYER_SHADOW_HEIGHT = 300.0f;
	float PLAYER_SHADOW_SIZE = 30.0f;

	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3] = { VERTEX3D(), VERTEX3D(), VERTEX3D() };
	VECTOR SlideVec;
	int ModelHandle;

	// ライティングを無効にする
	SetUseLighting(FALSE);

	// Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	// テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// 影を落とすモデルの数だけ繰り返し
	for (const auto c : colliders_)
	{

		// チェックするモデルは、jが0の時はステージモデル、1以上の場合はコリジョンモデル
		ModelHandle = c->modelId_;

		// プレイヤーの直下に存在する地面のポリゴンを取得
		HitResDim = MV1CollCheck_Capsule(
			ModelHandle, -1,
			transform_.pos, VAdd(transform_.pos, { 0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f }), PLAYER_SHADOW_SIZE);

		// 頂点データで変化が無い部分をセット
		Vertex[0].dif = GetColorU8(255, 255, 255, 255);
		Vertex[0].spc = GetColorU8(0, 0, 0, 0);
		Vertex[0].su = 0.0f;
		Vertex[0].sv = 0.0f;
		Vertex[1] = Vertex[0];
		Vertex[2] = Vertex[0];

		// 球の直下に存在するポリゴンの数だけ繰り返し
		HitRes = HitResDim.Dim;
		for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
		{
			// ポリゴンの座標は地面ポリゴンの座標
			Vertex[0].pos = HitRes->Position[0];
			Vertex[1].pos = HitRes->Position[1];
			Vertex[2].pos = HitRes->Position[2];

			// ちょっと持ち上げて重ならないようにする
			SlideVec = VScale(HitRes->Normal, 0.5f);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			// ポリゴンの不透明度を設定する
			Vertex[0].dif.a = 0;
			Vertex[1].dif.a = 0;
			Vertex[2].dif.a = 0;
			if (HitRes->Position[0].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[0].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[0].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			if (HitRes->Position[1].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[1].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[1].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			if (HitRes->Position[2].y > transform_.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[2].dif.a = static_cast<int>(roundf(128.0f * (1.0f - fabs(HitRes->Position[2].y - transform_.pos.y) / PLAYER_SHADOW_HEIGHT)));

			// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
			Vertex[0].u = (HitRes->Position[0].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[0].v = (HitRes->Position[0].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].u = (HitRes->Position[1].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].v = (HitRes->Position[1].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].u = (HitRes->Position[2].x - transform_.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].v = (HitRes->Position[2].z - transform_.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

			// 影ポリゴンを描画
			DrawPolygon3D(Vertex, 1, imgShadow_, TRUE);
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(HitResDim);
	}

	// ライティングを有効にする
	SetUseLighting(TRUE);

	// Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);

}

const VECTOR& Enemy::GetPos(void) const
{
	return transform_.pos;
}

void Enemy::SetPos(const VECTOR& pos)
{
	transform_.pos = pos;
}

const Transform* Enemy::GetTransform() const
{
	return &transform_;
}


void Enemy::ProcessMove(void)
{
	targetTimer_ -= scnMng_.GetDeltaTime();

	// 目標地点を再設定
	if (targetTimer_ <= 0.0f)
	{
		targetTimer_ = 3.0f; // 2秒ごとに変更

		targetPos_.x = (float)(rand() % 2000 - 1000);
		targetPos_.y = (float)(rand() % 500 + 100);
		targetPos_.z = (float)(rand() % 2000 - 1000);
	}

	VECTOR dir = VSub(targetPos_, transform_.pos);

	float dist = VSize(dir);

	if (dist > 1.0f)
	{
		dir = VNorm(dir);

		speed_ = 2.0f;
		movePow_ = VScale(dir, speed_);

		// 向きを移動方向に合わせる
		float rotY = atan2f(dir.x, dir.z);
		SetGoalRotate(rotY);
	}
	else
	{
		movePow_ = VGet(0.0f, 0.0f, 0.0f);
	}
}

void Enemy::SetGoalRotate(double rotRad)
{
	//VECTOR cameraRot = SceneManager::GetInstance().GetCamera()->GetAngles();
	Quaternion axis = Quaternion::AngleAxis((double)rotRad, AsoUtility::AXIS_Y);

	// 現在設定されている回転との角度差を取る
	double angleDiff = Quaternion::Angle(axis, goalQuaRot_);

	// しきい値
	if (angleDiff > 0.1)
	{
		stepRotTime_ = TIME_ROT;
	}

	goalQuaRot_ = axis;
}

void Enemy::Rotate(void)
{
	stepRotTime_ -= scnMng_.GetDeltaTime();

	// 回転の球面補間
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);
}

void Enemy::Collision(void)
{
	// 現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	// 衝突(カプセル)
	CollisionCapsule();

	// 衝突(重力)
	CollisionGravity();

	// 移動
	transform_.pos = movedPos_;
}


void Enemy::CollisionGravity(void)
{

	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力方向の反対
	VECTOR dirUpGravity = AsoUtility::DIR_U;

	// 重力の強さ
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	float checkPow = 10.0f;
	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));
	for (const auto c : colliders_)
	{

		// 地面との衝突
		auto hit = MV1CollCheck_Line(
			c->modelId_, -1, gravHitPosUp_, gravHitPosDown_);

	}

}

void Enemy::CollisionCapsule(void)
{
	// 現在座標と移動後座標
	VECTOR startPos = transform_.pos;
	VECTOR targetPos = movedPos_;
	VECTOR delta = VSub(targetPos, startPos);

	// 距離
	float dist = VSize(delta);

	// 移動がほぼ無い場合は従来通り一回だけ判定


		// カプセルを移動させる
	Transform trans = Transform(transform_);
	trans.pos = movedPos_;
	trans.Update();
	Capsule cap = Capsule(*capsule_, trans);

	// カプセルとの衝突判定
	for (const auto c : colliders_)
	{

		auto hits = MV1CollCheck_Capsule(
			c->modelId_, -1,
			cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());

		for (int i = 0; i < hits.HitNum; i++)
		{

			auto hit = hits.Dim[i];

			for (int tryCnt = 0; tryCnt < 10; tryCnt++)
			{

				int pHit = HitCheck_Capsule_Triangle(
					cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]);

				if (pHit)
				{
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 0.1f));
					// カプセルを移動させる
					trans.pos = movedPos_;
					trans.Update();

					continue;
				}

				break;

			}

		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);

	}

	return;
}

void Enemy::CalcGravityPow(void)
{
	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力の強さ
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	// 重力
	VECTOR gravity = VScale(dirGravity, gravityPow);

}

bool Enemy::IsEndLanding(void)
{
	bool ret = true;

	// アニメーションが終了しているか
	if (animationController_->IsEnd())
	{
		return ret;
	}

	return false;
}