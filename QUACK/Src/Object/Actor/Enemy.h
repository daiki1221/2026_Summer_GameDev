#pragma once
#include <DxLib.h>
#include <memory>
#include "ActorBase.h"
class AnimationController;
class Collider;
class Capsule;

class Enemy : public ActorBase
{

public:

	// スピード
	static constexpr float SPEED_MOVE = 3.0f;
	static constexpr float SPEED_RUN = 10.0f;

	// 回転完了までの時間
	static constexpr float TIME_ROT = 1.0f;

	// ジャンプ力
	static constexpr float POW_JUMP = 35.0f;

	// ジャンプ受付時間
	static constexpr float TIME_JUMP_IN = 0.5f;

	// アニメーションの再生速度
	static constexpr float SPEED_ANIM = 20.0f;

	int hp_;
	float hitColorTimer_;
	bool isHit_;
	int color;

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		RUN,
		FAST_RUN,
		JUMP,
		MAX,
	};

	// コンストラクタ
	Enemy(void);
	// デストラクタ
	~Enemy(void);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	// 衝突判定に用いられるコライダ制御
	void AddCollider(Collider* collider);
	void ClearCollider(void);

	// 衝突用カプセルの取得
	const Capsule* GetCapsule(void) const;


	// 座標の取得
	const VECTOR& GetPos(void) const;

	void SetPos(const VECTOR& pos);

	// Transform の取得
	const Transform* GetTransform() const;

	void Damage(int damage);
	bool IsDead(void) const;


private:

	VECTOR targetPos_;
	float targetTimer_;

	// アニメーション制御
	std::unique_ptr<AnimationController> animationController_;

	// 移動スピード
	float speed_;

	// 移動方向
	VECTOR moveDir_;

	// 移動量
	VECTOR movePow_;

	// 移動後の座標
	VECTOR movedPos_;

	// 回転
	Quaternion playerRotY_;
	Quaternion goalQuaRot_;
	float stepRotTime_;

	// 衝突判定に用いられるコライダ
	std::vector<Collider*> colliders_;
	std::unique_ptr<Capsule> capsule_;
	Player* player_;

	// 衝突チェック
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	// 丸影
	int imgShadow_;

	int followIndex_;

	

	void InitAnimation(void);

	// 更新ステップ
	void UpdateNone(void);
	void UpdatePlay(void);

	// 描画系
	void DrawShadow(void);

	// 操作
	void ProcessMove(void);

	// 回転
	void SetGoalRotate(double rotRad);
	void Rotate(void);

	// 衝突判定
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);

	// 移動量の計算
	void CalcGravityPow(void);

	// 着地モーション終了
	bool IsEndLanding(void);

};