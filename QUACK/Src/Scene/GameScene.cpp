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
	player_ = std::make_shared<Player>();
	player_->Init();

	enemy_ = std::make_unique<Enemy>();
	enemy_->Init();

	for (int i = 0; i < 3; i++)
	{
		auto duck = std::make_shared<Duckling>();

		duck->Init();

		duck->SetFollowIndex(i);

		duck->SetPos({
			i * 50.0f,
			-30.0f,
			1000.0f
			});

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


	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
	stage_->Update();

	player_->Update();

	enemy_->Update();

	for (auto& duck : duckling_)
	{
		duck->Update();
	}

	VECTOR nestPos = { -100.0f, -100.0f, 300.0f };
	const float NEST_RADIUS = 50.0f;           // 判定用半径
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

	
}

void GameScene::Draw(void)
{
	// ステージの描画
	stage_->Draw();

	player_->Draw();

	enemy_->Draw();

	for (auto& duck : duckling_)
	{
		duck->Draw();
	}

	// 残り時間を描画
	int timeDisplay = static_cast<int>(time_);
	DrawFormatString(50, 65, GetColor(255, 255, 255), "TIME: %d", timeDisplay);

}

