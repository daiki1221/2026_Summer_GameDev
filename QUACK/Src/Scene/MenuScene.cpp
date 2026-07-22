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
        printfDx("guide_image.png ‚Ì“Ç‚İ‚İ‚É¸”s‚µ‚Ü‚µ‚½\n");
    }
 
}

void MenuScene::Update(void)
{
    int mouseX, mouseY;
    GetMousePoint(&mouseX, &mouseY);

    if (inputManager_->IsTrgDown(KEY_INPUT_SPACE) ||
        (GetMouseInput() & MOUSE_INPUT_LEFT))
    {
        sceneManager_->ReturnPrevScene(); // Œ³‚ÌƒV[ƒ“‚É–ß‚·
    }

}

void MenuScene::Draw(void)
{
    DrawGraph(0, 0, guideImage_, true);
}
