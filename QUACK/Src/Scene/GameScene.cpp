#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/Common/Stage.h"
#include "../Object/Actor/Player.h"
#include "../Object/Actor/Planet.h"
#include "../Object/Actor/Duckling.h"
#include "../Object/Actor/Enemy.h"
#include "../Object/Actor/Bullet.h"
#include "GameScene.h"

GameScene::GameScene(void)
{
	player_ = nullptr;
	stage_ = nullptr;
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{

	pattern = rand() % patterns.size();

	player_ = std::make_shared<Player>();
	player_->Init();

	enemy_ = std::make_unique<Enemy>();
	enemy_->Init();

	player_->SetEnemy(enemy_.get());

	for (int i = 0; i < 3; i++)
	{
		auto duck = std::make_shared<Duckling>();

		duck->Init();

		duck->SetFollowIndex(i);

		// 初期座標の設定
		duck->SetPos(patterns[pattern][i]);

		duck->SetPlayer(player_.get());

		duckling_.push_back(duck);
	}
	// 追従設定
	duckling_[0]->SetPlayer(player_.get());

	for (int i = 1; i < duckling_.size(); i++)
	{
		duckling_[i]->SetPlayer(player_.get());

		duckling_[i]->SetPrevDuckling(
			duckling_[i - 1].get());
	}


	// ステージの生成と初期化
	stage_ = std::make_unique<Stage>(player_, duckling_);
	stage_->Init();

	
	// ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	SceneManager::GetInstance().GetCamera()->SetFollow(player_->GetTransform());
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FOLLOW);

	time_ = 300.0f;

}

void GameScene::Update(void)
{
	time_ -= 1.0f / 60.0f;

	if (time_ <= 0.0f)
	{
		SceneManager::GetInstance().SetResult(ResultScene::RESULT_TYPE::OVER);
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::RESULT);
	}


	stage_->Update();

	player_->Update();
	
	enemy_->Update();

	for (auto& duck : duckling_)
	{
		duck->Update();
	}

	VECTOR nestPos = { -100.0f, -100.0f, 300.0f };
	const float NEST_RADIUS = 100.0f;           // 判定用半径
	int reachCount = 0;

	for (auto& duck : duckling_)
	{
		float dist = VSize(VSub(duck->GetPos(), nestPos));

		if (dist < NEST_RADIUS)
		{
			reachCount++;
		}
	}

	if (reachCount >= 3)
	{
		SceneManager::GetInstance().OnReachNest();
	}

	auto const& bullet_ = player_->GetBullet();

	for (auto& bullet : bullet_)
	{

		VECTOR diff =
			VSub(enemy_->GetPos(),
				bullet->GetPos());

		float distance = VSize(diff);

		if (distance < 100.0f)
		{
			enemy_->Damage(10);
			bullet->SetDead();
		}
	}

	if (enemy_->IsDead())
	{
		enemy_->SetPos({ -1000.0f, 500.0f, 1000.0f });
	}

	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_TAB))
	{
		SceneManager::GetInstance().OpenMenu();
	}

}

void GameScene::Draw(void)
{

	// ステージの描画
	stage_->Draw();

	enemy_->Draw();

	player_->Draw();

	

	for (auto& duck : duckling_)
	{
		duck->Draw();
	}

	// 残り時間を描画
	int timeDisplay = static_cast<int>(time_);
	DrawFormatString(50, 65, GetColor(255, 255, 255), "TIME: %d", timeDisplay);

	// プレイヤー座標表示
	VECTOR pos = player_->GetPos();

	DrawFormatString(
		50, 90,
		GetColor(255, 255, 0),
		"POS X: %.2f Y: %.2f Z: %.2f",
		pos.x, pos.y, pos.z);


}

