#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include "ActorBase.h"
#include "FeatherParticle.h"
class AnimationController;
class Collider;
class Capsule;
class Bullet;
class Enemy;
class FeatherParticle;

class Player : public ActorBase
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

	// 状態
	enum class STATE
	{
		NONE,
		PLAY,
		WARP_RESERVE,
		WARP_MOVE,
		DEAD,
		VICTORY,
		END
	};

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		RUN,
		FAST_RUN,
		JUMP,
		ATTACK,
		MAX,
	};

	// コンストラクタ
	Player(void);
	// デストラクタ
	~Player(void);

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

	// Transform の取得
	const Transform* GetTransform() const;

	VECTOR GetForward(void) const;

	void WaterBullet(const VECTOR& dir);

	const std::vector<std::shared_ptr<Bullet>>& GetBullet() const;

	void SetEnemy(Enemy* enemy);

	void Damage(int damage);
	bool IsDead(void) const;

private:
	// アニメーション制御
	std::unique_ptr<AnimationController> animationController_;

	std::vector<std::shared_ptr<Bullet>> bullet_;

	Enemy* enemy_;

	// 状態管理
	STATE state_;

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

	// ジャンプ量
	VECTOR jumpPow_;

	// ジャンプ判定
	bool isJump_;

	// ジャンプの入力受付時間
	float stepJump_;

	VECTOR aimDir_;
	bool isAiming_;

	// 衝突判定に用いられるコライダ
	std::vector<Collider*> colliders_;
	std::unique_ptr<Capsule> capsule_;
	
	std::vector<FeatherParticle> feathers_;

	// 衝突チェック
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	// 丸影
	int imgShadow_;

	bool isAttack_;
	float attackTime_;

	static constexpr float TIME_ATTACK = 0.5f;

	int hp_;
	bool isHit_;
	float hitColorTimer_;

	void InitAnimation(void);

	// 状態遷移
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStatePlay(void);

	// 更新ステップ
	void UpdateNone(void);
	void UpdatePlay(void);

	// 描画系
	void DrawShadow(void);

	// 操作
	void ProcessMove(void);
	void ProcessJump(void);
	void ProcessAtttack(void);

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
