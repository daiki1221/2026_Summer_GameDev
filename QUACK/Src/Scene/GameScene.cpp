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
#include "../Manager/ResourceManager.h"
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
	// 乱数の初期化
	pattern = rand() % patterns.size();

	// プレイヤーの生成と初期化
	player_ = std::make_shared<Player>();
	player_->Init();
	// 敵の生成と初期化
	enemy_ = std::make_unique<Enemy>();
	enemy_->Init();
	// プレイヤーに敵を設定
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
	// 残り時間の初期化
	time_ = 300.0f;
	// 巣を目指すメッセージの初期化
	isNestMessage_ = false;
	isNestMessageDisplayed_ = false;
	nestMessageTimer_ = 0.0f;

	imgNestMessage_ = -1;
	imgNestMessage_ =
		ResourceManager::GetInstance().Load(ResourceManager::SRC::GUIDE).handleId_;
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

	// 追従中のアヒルの数をカウント
	int followingCount = 0;
	for (auto& duck : duckling_)
	{
		if (duck->IsFollowing())
		{
			followingCount++;
		}
	}
	// 追従中のアヒルの数が3匹で、巣に到達するようにメッセージを表示
	if (followingCount >= 3 &&
		!isNestMessageDisplayed_)
	{
		isNestMessage_ = true;
		isNestMessageDisplayed_ = true;
		nestMessageTimer_ = 7.0f;
	}
	// メッセージ表示中のタイマー処理
	if (isNestMessage_)
	{
		nestMessageTimer_ -= 1.0f / 60.0f;

		if (nestMessageTimer_ <= 0.0f)
		{
			isNestMessage_ = false;
		}
	}

	// 巣に到達したアヒルの数が3匹以上の場合クリア処理へ
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

	if (ins.IsTrgDown(KEY_INPUT_F))
	{
		for (auto& duck : duckling_)
		{
			if (duck->IsFollowing())
				continue;

			if (!duck->IsFoundPlayer())
				continue;

			duck->StartFollowing();
			break;
		}
	}


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

	if (isNestMessage_)
	{
		DrawGraph(
			Application::SCREEN_SIZE_X / 2 - 200,
			50,
			imgNestMessage_,
			TRUE);

		DrawString(
			Application::SCREEN_SIZE_X / 2 - 80,
			230,
			"3羽そろった！巣へ帰ろう！",
			GetColor(255, 255, 0));
	}
}

