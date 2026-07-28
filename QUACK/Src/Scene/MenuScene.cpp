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

    guideImage_[0] = LoadGraph("Data/Image/Menu/sousa.png");
    guideImage_[1] = LoadGraph("Data/Image/Menu/sousa2.png");

    if (guideImage_[0] == -1 || guideImage_[1] == -1)
    {
        printfDx("‘€ìà–¾‰æ‘œ‚Ì“Ç‚Ýž‚ÝŽ¸”s\n");
    }

    isSecondPage_ = false;
 
}

void MenuScene::Update(void)
{
	auto& ins = InputManager::GetInstance();

    InputManager::JOYPAD_IN_STATE padState =
		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

    int mouseX, mouseY;
    GetMousePoint(&mouseX, &mouseY);

    bool push =
        inputManager_->IsTrgDown(KEY_INPUT_SPACE) ||
        inputManager_->IsMouseTrgDown(MOUSE_INPUT_LEFT) ||
        ins.IsPadBtnTrgDown(
            InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::RIGHT);


    if (push)
    {
        // 1–‡–Ú‚È‚ç2–‡–Ú‚Ö
        if (!isSecondPage_)
        {
            isSecondPage_ = true;
        }
        // 2–‡–Ú‚È‚ç–ß‚é
        else
        {
            sceneManager_->ReturnPrevScene();
        }
    }

}

void MenuScene::Draw(void)
{
    if (isSecondPage_)
    {
        DrawGraph(
            0,
            0,
            guideImage_[1],
            true);
    }
    else
    {
        DrawGraph(
            0,
            0,
            guideImage_[0],
            true);
    }
}