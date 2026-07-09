#include <DxLib.h>
#include <string>
#include <memory>
#include <algorithm>
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
#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"

Player::Player(void)
{
	animationController_ = nullptr;
	state_ = STATE::NONE;

	speed_ = 0.0f;
	moveDir_ = AsoUtility::VECTOR_ZERO;
	movePow_ = AsoUtility::VECTOR_ZERO;
	movedPos_ = AsoUtility::VECTOR_ZERO;

	playerRotY_ = Quaternion();
	goalQuaRot_ = Quaternion();
	stepRotTime_ = 0.0f;

	jumpPow_ = AsoUtility::VECTOR_ZERO;
	isJump_ = false;
	stepJump_ = 0.0f;

	// 衝突チェック
	gravHitPosDown_ = AsoUtility::VECTOR_ZERO;
	gravHitPosUp_ = AsoUtility::VECTOR_ZERO;

	imgShadow_ = -1;

	isAttack_ = false;
	attackTime_ = 0.0f;

	enemy_ = nullptr;

	capsule_ = nullptr;

}

Player::~Player(void)
{
}

void Player::Init(void)
{
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::PLAYER));
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.pos = { 0.0f, -30.0f, 0.0f };
	transform_.quaRot = Quaternion();
	transform_.quaRotLocal =
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	transform_.Update();

	// アニメーションの設定
	InitAnimation();

	// カプセルコライダ
	capsule_ = std::make_unique<Capsule>(transform_);
	capsule_->SetLocalPosTop({ 0.0f, 110.0f, 0.0f });
	capsule_->SetLocalPosDown({ 0.0f, 30.0f, 0.0f });
	capsule_->SetRadius(20.0f);

	// 丸影画像
	imgShadow_ = resMng_.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;

	aimDir_ = AsoUtility::VECTOR_ZERO;
	isAiming_ = false;

	hp_ = 150;
	isHit_ = false;
	hitColorTimer_ = 0.0f;

	// 初期状態
	ChangeState(STATE::PLAY);
}

void Player::Update(void)
{
	// 更新ステップ
	switch (state_)
	{
	case Player::STATE::NONE:
		UpdateNone();
		break;
	case Player::STATE::PLAY:
		UpdatePlay();
		break;
	}

	for (auto& bullet : bullet_)
	{
		bullet->Update();
	}

	// モデル制御更新
	transform_.Update();

	// アニメーション再生
	animationController_->Update();

	bullet_.erase(
		std::remove_if(
			bullet_.begin(),
			bullet_.end(),
			[](const auto& b)
			{
				return b->IsDead();
			}),
		bullet_.end());

	// ヒット時の色変化タイマー
	if (isHit_)
	{
		hitColorTimer_ -= scnMng_.GetDeltaTime();

		if (hitColorTimer_ <= 0.0f)
		{
			isHit_ = false;
		}
	}
	// 羽根パーティクルの更新
	for (auto& feather : feathers_)
	{
		feather.Update();
	}

	feathers_.erase(
		std::remove_if(
			feathers_.begin(),
			feathers_.end(),
			[](const FeatherParticle& f)
			{
				return f.IsDead();
			}),
		feathers_.end());

	// HPが0になったらゲームオーバー
	if (IsDead())
	{
		SceneManager::GetInstance().SetResult(ResultScene::RESULT_TYPE::OVER);
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::RESULT);
	}

}

void Player::Draw(void)
{
	for (auto& bullet : bullet_)
	{
		bullet->Draw();
	}

	// 丸影描画
	DrawShadow();

	// エイムカーソル描画
	if (isAiming_)
	{
		DrawCircle(
			Application::SCREEN_SIZE_X / 2,
			Application::SCREEN_SIZE_Y / 1.9,
			40,                    // 半径
			GetColor(0, 255, 255),   // 水色
			FALSE                  // 枠だけ
		);
	}

	// ヒット時の色変化
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

	// HP表示
	DrawFormatString(
		20, 20,
		GetColor(255, 255, 255),
		"HP : %d",
		hp_);

	// モデルの描画
	MV1DrawModel(transform_.modelId);
	// 羽根パーティクルの描画
	for (auto& feather : feathers_)
	{
		feather.Draw();
	}
}

void Player::AddCollider(Collider* collider)
{
	colliders_.push_back(collider);
}

void Player::ClearCollider(void)
{
	colliders_.clear();
}

const Capsule* Player::GetCapsule(void) const
{
	return capsule_.get();
}

void Player::InitAnimation(void)
{

	std::string path = Application::PATH_MODEL + "Player/";
	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	animationController_->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::RUN, path + "duck_walk.mv1", 30.0f);
	animationController_->Add((int)ANIM_TYPE::FAST_RUN, path + "duck_walk.mv1", 40.0f);
	animationController_->Add((int)ANIM_TYPE::JUMP, path + "Jump.mv1", 60.0f);
	/*animationController_->Add((int)ANIM_TYPE::WARP_PAUSE, path + "WarpPose.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FLY, path + "Flying.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FALLING, path + "Falling.mv1", 80.0f);
	animationController_->Add((int)ANIM_TYPE::VICTORY, path + "Victory.mv1", 60.0f);
	*/
	animationController_->Play((int)ANIM_TYPE::IDLE);

}

void Player::ChangeState(STATE state)
{
	// 状態変更
	state_ = state;

	// 各状態遷移の初期処理
	switch (state_)
	{
	case Player::STATE::NONE:
		ChangeStateNone();
		break;
	case Player::STATE::PLAY:
		ChangeStatePlay();
		break;
	}
}

void Player::ChangeStateNone(void)
{
}

void Player::ChangeStatePlay(void)
{
}

void Player::UpdateNone(void)
{
}

void Player::UpdatePlay(void)
{

	// 移動処理
	ProcessMove();

	// ジャンプ処理
	ProcessJump();

	// 攻撃処理
	ProcessAtttack();

	// 移動方向に応じた回転
	Rotate();

	// 重力による移動量
	CalcGravityPow();

	// 衝突判定
	Collision();



	// 回転させる
	transform_.quaRot = playerRotY_;

}

void Player::DrawShadow(void)
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

const VECTOR& Player::GetPos(void) const
{
	return transform_.pos;
}

const Transform* Player::GetTransform() const
{
	return &transform_;
}

void Player::ProcessMove(void)
{

	auto& ins = InputManager::GetInstance();

	// 移動量をゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// X軸回転を除いた、重力方向に垂直なカメラ角度(XZ平面)を取得
	Quaternion cameraRot = SceneManager::GetInstance().GetCamera()->GetQuaRotOutX();

	// 回転したい角度
	double rotRad = 0;

	VECTOR dir = AsoUtility::VECTOR_ZERO;
	
	// カメラ方向に前進したい
	if (ins.IsNew(KEY_INPUT_W))
	{
		rotRad = AsoUtility::Deg2RadD(0.0);
		dir = cameraRot.GetForward();
	}

	// カメラ方向から後退したい
	if (ins.IsNew(KEY_INPUT_S))
	{
		rotRad = AsoUtility::Deg2RadD(180.0);
		dir = cameraRot.GetBack();
	}

	// カメラ方向から右側へ移動したい
	if (ins.IsNew(KEY_INPUT_D))
	{
		rotRad = AsoUtility::Deg2RadD(90.0);
		dir = cameraRot.GetRight();
	}

	// カメラ方向から左側へ移動したい
	if (ins.IsNew(KEY_INPUT_A))
	{
		rotRad = AsoUtility::Deg2RadD(270.0);
		dir = cameraRot.GetLeft();
	}

	if (!AsoUtility::EqualsVZero(dir) && (isJump_ || IsEndLanding())) {

		// 移動処理
		speed_ = SPEED_MOVE;
		if (ins.IsNew(KEY_INPUT_LSHIFT))
		{
			speed_ = SPEED_RUN;
		}
		moveDir_ = dir;
		movePow_ = VScale(dir, speed_);

		// 回転処理
		SetGoalRotate(rotRad);

		if (!isJump_ && IsEndLanding())
		{
			// アニメーション
			if (ins.IsNew(KEY_INPUT_LSHIFT))
			{
				animationController_->Play((int)ANIM_TYPE::FAST_RUN);
			}
			else
			{
				animationController_->Play((int)ANIM_TYPE::RUN);
			}
		}

	}
	else
	{
		if (!isJump_ && IsEndLanding())
		{
			animationController_->Play((int)ANIM_TYPE::IDLE);
		}
	}

}

void Player::ProcessJump(void)
{
	bool isHit = CheckHitKey(KEY_INPUT_BACKSLASH);

	// ジャンプ
	if (isHit && (isJump_ || IsEndLanding()))
	{

		if (!isJump_)
		{
			// 制御無しジャンプ
			//mAnimationController->Play((int)ANIM_TYPE::JUMP);
			// ループしないジャンプ
			//mAnimationController->Play((int)ANIM_TYPE::JUMP, false);
			// 切り取りアニメーション
			//mAnimationController->Play((int)ANIM_TYPE::JUMP, false, 13.0f, 24.0f);
			// 無理やりアニメーション
			animationController_->Play((int)ANIM_TYPE::JUMP, true, 13.0f, 25.0f);
			animationController_->SetEndLoop(23.0f, 25.0f, 5.0f);
		}

		isJump_ = true;

		// ジャンプの入力受付時間をヘラス
		stepJump_ += scnMng_.GetDeltaTime();
		if (stepJump_ < TIME_JUMP_IN)
		{
			jumpPow_ = VScale(AsoUtility::DIR_U, POW_JUMP);
		}

	}

	// ボタンを離したらジャンプ力に加算しない
	if (!isHit)
	{
		stepJump_ = TIME_JUMP_IN;
	}

}

void Player::ProcessAtttack(void)
{
	auto& ins = InputManager::GetInstance();

	// 右クリック：照準ON（表示だけ）
	if (ins.IsTrgMouseRight())
	{
		isAiming_ = !isAiming_; // トグルでもOK
	}

	// 左クリック：発射（常に現在カメラ方向）
	if (ins.IsTrgMouseLeft())
	{
		VECTOR dir = SceneManager::GetInstance()
			.GetCamera()
			->GetForward();

		WaterBullet(dir);
	}
}

void Player::SetGoalRotate(double rotRad)
{
	VECTOR cameraRot = SceneManager::GetInstance().GetCamera()->GetAngles();
	Quaternion axis = Quaternion::AngleAxis((double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);

	// 現在設定されている回転との角度差を取る
	double angleDiff = Quaternion::Angle(axis, goalQuaRot_);

	// しきい値
	if (angleDiff > 0.1)
	{
		stepRotTime_ = TIME_ROT;
	}

	goalQuaRot_ = axis;
}

void Player::Rotate(void)
{
	stepRotTime_ -= scnMng_.GetDeltaTime();

	// 回転の球面補間
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);
}

void Player::Collision(void)
{
	// 現在座標を起点に移動後座標を決める
	movedPos_ = VAdd(transform_.pos, movePow_);

	// 衝突(カプセル)
	CollisionCapsule();

	// 衝突(重力)
	CollisionGravity();

	// ステージ範囲制限
	if (movedPos_.x < -4100.0f)
	{
		movedPos_.x = -4100.0f;
	}
	else if (movedPos_.x > 3900.0f)
	{
		movedPos_.x = 3900.0f;
	}

	if (movedPos_.z < -4300.0f)
	{
		movedPos_.z = -4300.0f;
	}
	else if (movedPos_.z > 4100.0f)
	{
		movedPos_.z = 4100.0f;
	}

	// 移動
	transform_.pos = movedPos_;
}

void Player::CollisionGravity(void)
{
	// ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);

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

		// 最初は上の行のように実装して、木の上に登ってしまうことを確認する
		//if (hit.HitFlag > 0)
		if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.9f)
		{

			// 衝突地点から、少し上に移動
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));

			// ジャンプリセット
			jumpPow_ = AsoUtility::VECTOR_ZERO;
			stepJump_ = 0.0f;

			if (isJump_)
			{
				// 着地モーション
				animationController_->Play(
					(int)ANIM_TYPE::JUMP, false, 29.0f, 45.0f, false, true);
			}

			isJump_ = false;

		}

	}

}

void Player::CollisionCapsule(void)
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

void Player::CalcGravityPow(void)
{
	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	// 重力の強さ
	float gravityPow = Planet::DEFAULT_GRAVITY_POW;

	// 重力
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

	// 最初は実装しない。地面と突き抜けることを確認する。
	// 内積
	float dot = VDot(dirGravity, jumpPow_);
	if (dot >= 0.0f)
	{
		// 重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
		jumpPow_ = gravity;
	}

}

bool Player::IsEndLanding(void)
{
	bool ret = true;

	// アニメーションがジャンプではない
	if (animationController_->GetPlayType() != (int)ANIM_TYPE::JUMP)
	{
		return ret;
	}

	// アニメーションが終了しているか
	if (animationController_->IsEnd())
	{
		return ret;
	}

	return false;
}

void Player::WaterBullet(const VECTOR& dir)
{
	VECTOR pos = transform_.pos;

	pos = VAdd(pos, { 0.0f, 40.0f, 0.0f });   // 上へ40
	pos = VAdd(pos, VScale(dir, 50.0f));    // 前へ50

	bullet_.push_back(
		std::make_shared<Bullet>(pos, VNorm(dir))
	);

	animationController_->Play((int)ANIM_TYPE::ATTACK, false);
}

const std::vector<std::shared_ptr<Bullet>>& Player::GetBullet() const
{
	return bullet_;
}

void Player::SetEnemy(Enemy* enemy)
{
	enemy_ = enemy;
}

void Player::Damage(int damage)
{
	hp_ -= damage;

	if (hp_ < 0)
	{
		hp_ = 0;
	}

	isHit_ = true;
	hitColorTimer_ = 1.0f;

	for (int i = 0; i < 5; i++)
	{
		FeatherParticle f;
		VECTOR pos = transform_.pos;
		pos.y += 30.0f;
		// ランダムに少しずらす
		pos.x += (rand() % 40 - 20) * 0.1f;
		pos.z += (rand() % 40 - 20) * 0.1f;

		f.Init(pos);
		feathers_.push_back(f);
	}
}

bool Player::IsDead(void) const
{
	return hp_ <= 0;
}

VECTOR Player::GetForward(void) const
{
	return transform_.quaRot.GetForward();
}













