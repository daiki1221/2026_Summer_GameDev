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

	duckling_ = std::make_shared<Duckling>();
	duckling_->Init();

	duckling_->SetPlayer(player_.get());

	// ステージの生成と初期化
	stage_ = std::make_unique<Stage>(player_, duckling_);
	stage_->Init();

	
	// ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	SceneManager::GetInstance().GetCamera()->SetFollow(player_->GetTransform());
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FOLLOW);

	time_ = 30.0f;

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

	duckling_->Update();

	VECTOR nestPos = { -100.0f, -100.0f, 300.0f };
	const float NEST_RADIUS = 50.0f;           // 判定用半径

	if (duckling_) {
		float dist = VSize(VSub(duckling_->GetPos(), nestPos));
		if (dist < NEST_RADIUS) {
			// ひなが巣に到達！
			SceneManager::GetInstance().OnReachNest(); // → クリア処理へ
		}
	}


	
}

void GameScene::Draw(void)
{
	// ステージの描画
	stage_->Draw();

	player_->Draw();

	duckling_->Draw();

	// 残り時間を描画
	int timeDisplay = static_cast<int>(time_);
	DrawFormatString(50, 65, GetColor(255, 255, 255), "TIME: %d", timeDisplay);

}

