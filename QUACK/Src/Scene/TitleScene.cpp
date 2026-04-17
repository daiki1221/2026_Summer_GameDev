#include <DxLib.h>
#include "TitleScene.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Application.h"

TitleScene::TitleScene(void)
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
    image_ = LoadGraph("Data/Model/Title/taitle.png");

}

void TitleScene::Update(void)
{
    if (InputManager::GetInstance().IsNew(KEY_INPUT_SPACE)) {
        SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
    }
}

void TitleScene::Draw(void)
{
    DrawRotaGraph(
        Application::SCREEN_SIZE_X / 2,
        Application::SCREEN_SIZE_Y / 1.7,
        1.0f, 0.0, image_, true);
}

void TitleScene::Release(void)
{
}