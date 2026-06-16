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
#include "Duckling.h"


Duckling::Duckling(void)
{

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

	followIndex_ = 0;

	capsule_ = nullptr;


}

Duckling::~Duckling(void)
{
}

void Duckling::Init(void)
{
	transform_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::DUCKLING));
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

	// 丸影画像
	imgShadow_ = resMng_.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;

	
}

void Duckling::Update(void)
{
	UpdatePlay();
	// モデル制御更新
	transform_.Update();
	
}

void Duckling::Draw(void)
{
	// モデルの描画
	MV1DrawModel(transform_.modelId);

	// 丸影描画
	DrawShadow();
}

void Duckling::AddCollider(Collider* collider)
{
	colliders_.push_back(collider);
}

void Duckling::ClearCollider(void)
{
	colliders_.clear();
}

const Capsule* Duckling::GetCapsule(void) const
{
	return capsule_.get();
}

void Duckling::InitAnimation(void)
{

	std::string path = Application::PATH_MODEL + "Duckling/";
	animationController_ = std::make_unique<AnimationController>(transform_.modelId);
	animationController_->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::RUN, path + "Run.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::FAST_RUN, path + "FastRun.mv1", 20.0f);
	animationController_->Add((int)ANIM_TYPE::JUMP, path + "Jump.mv1", 60.0f);
	/*
	animationController_->Add((int)ANIM_TYPE::WARP_PAUSE, path + "WarpPose.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FLY, path + "Flying.mv1", 60.0f);
	animationController_->Add((int)ANIM_TYPE::FALLING, path + "Falling.mv1", 80.0f);
	animationController_->Add((int)ANIM_TYPE::VICTORY, path + "Victory.mv1", 60.0f);
	
	animationController_->Play((int)ANIM_TYPE::IDLE);*/

}

void Duckling::UpdateNone(void)
{
}

void Duckling::UpdatePlay(void)
{

	// 移動処理
	
	ProcessMove();

	// ジャンプ処理
	ProcessJump();

	// 移動方向に応じた回転
	Rotate();

	// 重力による移動量
	CalcGravityPow();

	// 衝突判定
	Collision();

	// 回転させる
	transform_.quaRot = playerRotY_;

}

void Duckling::DrawShadow(void)
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



const VECTOR& Duckling::GetPos(void) const
{
	return transform_.pos;
}

void Duckling::SetPos(const VECTOR& pos)
{
	transform_.pos = pos;
}

const Transform* Duckling::GetTransform() const
{
	return &transform_;
}

void Duckling::SetPlayer(Player* player)
{
	player_ = player;
}

void Duckling::SetFollowIndex(int index)
{
	followIndex_ = index;
}

void Duckling::ProcessMove(void)
{
	if (player_ == nullptr)
	{
		return;
	}

	// プレイヤーとの距離
	VECTOR toPlayer =
		VSub(player_->GetPos(), transform_.pos);

	float playerDist = VSize(toPlayer);

	// この距離以内で追従
	float activeDist = 400.0f;

	// 離れていたら何もしない
	if (playerDist > activeDist)
	{
		movePow_ = AsoUtility::VECTOR_ZERO;
		return;
	}

	VECTOR targetPos;

	if (followIndex_ == 0)
	{
		float backDist = 120.0f;

		VECTOR forward = player_->GetForward();

		targetPos =
			VSub(player_->GetPos(),
				VScale(forward, backDist));
	}
	else
	{
		if (prevDuckling_ == nullptr)
		{
			return;
		}

		float spacing = 80.0f;

		VECTOR dir =
			prevDuckling_->GetForward();

		targetPos =
			VSub(prevDuckling_->GetPos(),
				VScale(dir, spacing));
	}

	// 高さ無視
	targetPos.y = transform_.pos.y;

	// Duckling → 目標地点
	VECTOR toTarget =
		VSub(targetPos, transform_.pos);

	float dist = VSize(toTarget);

	// 止まる距離
	float stopDist = 2.0f;

	if (dist > stopDist)
	{
		VECTOR dir = VNorm(toTarget);

		speed_ = 3.0f;

		movePow_ = VScale(dir, speed_);

		double rotY = atan2(dir.x, dir.z);

		SetGoalRotate(rotY);
	}
	else
	{
		movePow_ = AsoUtility::VECTOR_ZERO;
	}
}

void Duckling::ProcessJump(void)
{
	bool isHit = CheckHitKey(KEY_INPUT_O);

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


void Duckling::SetGoalRotate(double rotRad)
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

void Duckling::Rotate(void)
{
	stepRotTime_ -= scnMng_.GetDeltaTime();

	// 回転の球面補間
	playerRotY_ = Quaternion::Slerp(
		playerRotY_, goalQuaRot_, (TIME_ROT - stepRotTime_) / TIME_ROT);
}

void Duckling::Collision(void)
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


void Duckling::CollisionGravity(void)
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

void Duckling::CollisionCapsule(void)
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

void Duckling::CalcGravityPow(void)
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

bool Duckling::IsEndLanding(void)
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

void Duckling::SetPrevDuckling(Duckling* duck)
{
	prevDuckling_ = duck;
}

VECTOR Duckling::GetForward(void) const
{
	return transform_.GetForward();
}




