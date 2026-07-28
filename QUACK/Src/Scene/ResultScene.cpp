#include <DxLib.h>
#include "ResultScene.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Scene/GameScene.h"

ResultScene::ResultScene(void)
{
	resultType_ = static_cast<RESULT_TYPE>(0);	// リザルトの種類
	sceneManager_ = nullptr;	// シーンマネージャー
	gameScene_ = nullptr;	// ゲームシーン
}

ResultScene::~ResultScene(void)
{
}

void ResultScene::Init(void)
{
	sceneManager_ = &SceneManager::GetInstance();

	GameClear_ = LoadGraph("Data/Image/GameClear.png");
	if (GameClear_ == -1) {
		printfDx("GameClear.png の読み込みに失敗しました\n");
	}
	
	GameOver_ = LoadGraph("Data/Image/GameOver.png");
	if (GameOver_ == -1) {
		printfDx("GameOver.png の読み込みに失敗しました\n");
	}

}

void ResultScene::Update(void)
{
	auto& ins = InputManager::GetInstance();

	if (InputManager::GetInstance().IsNew(KEY_INPUT_SPACE) ||
		ins.IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::RIGHT)) {
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}

void ResultScene::Draw(void)
{
	resultType_ = SceneManager::GetInstance().GetResult();	// リザルトの種類を取得

	switch (resultType_) {
	case ResultScene::RESULT_TYPE::OVER:
		DrawGraph(0, 0, GameOver_, TRUE);
		break;

	case ResultScene::RESULT_TYPE::CLEAR:
		DrawGraph(0, 0, GameClear_, TRUE);

		break;
	}

}
