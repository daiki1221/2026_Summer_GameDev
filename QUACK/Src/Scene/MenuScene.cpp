#include <DxLib.h>
#include "../Application.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include"GameScene.h"
#include "MenuScene.h"

MenuScene::MenuScene(void)
	:
	SceneBase()
{
}

MenuScene::~MenuScene(void)
{
}

void MenuScene::Init(void)
{
    inputManager_ = &InputManager::GetInstance();
    sceneManager_ = &SceneManager::GetInstance();

    guideImage_ = LoadGraph("Data/Image/Menu/sousa.png");
    if (guideImage_ == -1) {
        printfDx("guide_image.png の読み込みに失敗しました\n");
    }
 
}

void MenuScene::Update(void)
{
    int mouseX, mouseY;
    GetMousePoint(&mouseX, &mouseY);

    // spaceでメニューを閉じる
    if (inputManager_->IsTrgDown(KEY_INPUT_SPACE))
    {
        sceneManager_->ReturnPrevScene(); // 元のシーンに戻す
    }

}

void MenuScene::Draw(void)
{
    DrawGraph(0, 0, guideImage_, true);
}
