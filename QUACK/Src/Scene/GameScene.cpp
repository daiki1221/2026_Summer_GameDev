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

	// ステージの生成と初期化
	stage_ = std::make_unique<Stage>(player_);
	stage_->Init();
	
	// ステージの初期設定
	stage_->ChangeStage(Stage::NAME::MAIN_PLANET);

	SceneManager::GetInstance().GetCamera()->SetFollow(player_->GetTransform());
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FOLLOW);

}

void GameScene::Update(void)
{

	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
	stage_->Update();

	player_->Update();
	
}

void GameScene::Draw(void)
{
	// ステージの描画
	stage_->Draw();

	player_->Draw();

}

