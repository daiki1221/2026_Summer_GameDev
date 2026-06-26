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

    isGuideOpen_ = false;
    selectedIndex_ = 0;

    menuImage_ = LoadGraph("Data/Image/Menu/menu.png");
    if (menuImage_ == -1) {
        printfDx("menu.png の読み込みに失敗しました\n");
    }

    guideImage_ = LoadGraph("Data/Image/Menu/guide_image.png");
    if (guideImage_ == -1) {
        printfDx("guide_image.png の読み込みに失敗しました\n");
    }

    selectorImage_ = LoadGraph("Data/Image/Menu/select.png");
    if (selectorImage_ == -1) {
        printfDx("select.png の読み込みに失敗しました\n");
    }

    int imgTitle = LoadGraph("Data/Image/Menu/title.png");
    int imgGuide = LoadGraph("Data/Image/Menu/guide.png");
    int imgExit = LoadGraph("Data/Image/Menu/end.png");
    int imgTitlePushed = LoadGraph("Data/Image/Menu/title_pushed.png");
    int imgGuidePushed = LoadGraph("Data/Image/Menu/guide_pushed.png");
    int imgExitPushed = LoadGraph("Data/Image/Menu/end_pushed.png");

    // ボタンの座標・サイズを設定
    buttons_.push_back({ 450, 300, 300, 60, Button::TITLE, imgTitle, imgTitlePushed });
    buttons_.push_back({ 450, 400, 300, 60, Button::GUIDE, imgGuide, imgGuidePushed });
    buttons_.push_back({ 450, 500, 300, 60, Button::EXIT, imgExit, imgExitPushed });


}

void MenuScene::Update(void)
{
    int mouseX, mouseY;
    GetMousePoint(&mouseX, &mouseY);

    // ガイド画像表示中の入力処理
    if (isGuideOpen_) {
        if (inputManager_->IsClickMouseLeft() ||
            inputManager_->IsTrgDown(KEY_INPUT_SPACE) ||
            inputManager_->IsTrgDown(KEY_INPUT_TAB)) {
            isGuideOpen_ = false;
        }
        return;
    }

    for (size_t i = 0; i < buttons_.size(); ++i) {
        const auto& btn = buttons_[i];
        if (mouseX >= btn.x && mouseX <= btn.x + btn.w &&
            mouseY >= btn.y && mouseY <= btn.y + btn.h) {
            selectedIndex_ = static_cast<int>(i);
            break;  // 最初に一致したものを選ぶ
        }
    }

    // --- 矢印キーで選択移動 ---
    if (inputManager_->IsTrgDown(KEY_INPUT_UP)) {
        selectedIndex_--;
        if (selectedIndex_ < 0) selectedIndex_ = static_cast<int>(buttons_.size()) - 1;
    }
    if (inputManager_->IsTrgDown(KEY_INPUT_DOWN)) {
        selectedIndex_++;
        if (selectedIndex_ >= static_cast<int>(buttons_.size())) selectedIndex_ = 0;
    }

    // --- スペースキーで決定 ---
    if (inputManager_->IsTrgDown(KEY_INPUT_SPACE)) {
        const auto& btn = buttons_[selectedIndex_];
        switch (btn.type) {
        case Button::TITLE:
            sceneManager_->ChangeScene(SceneManager::SCENE_ID::TITLE);
            break;
        case Button::GUIDE:
            isGuideOpen_ = true;
            break;
        case Button::EXIT:
            DxLib_End();
            break;
        }
    }

    // 左クリック時
    if (inputManager_->IsClickMouseLeft())
    {
        for (const auto& btn : buttons_)
        {
            if (mouseX >= btn.x && mouseX <= btn.x + btn.w &&
                mouseY >= btn.y && mouseY <= btn.y + btn.h)
            {
                switch (btn.type)
                {
                case Button::TITLE:
                    sceneManager_->ChangeScene(SceneManager::SCENE_ID::TITLE);
                    break;
                case Button::GUIDE:
                    isGuideOpen_ = true;
                    break;
                case Button::EXIT:
                    // ゲーム終了
                    DxLib_End();
                    break;
                }
            }
        }
    }



    // tabでメニューを閉じる
    if (inputManager_->IsTrgDown(KEY_INPUT_TAB))
    {
        sceneManager_->ReturnPrevScene(); // 元のシーンに戻す
    }

}

void MenuScene::Draw(void)
{
    if (isGuideOpen_) {
        // 背景を半透明で暗くする
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);
        DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, GetColor(0, 0, 0), true);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        // 画面中央にガイド画像を描画
        int imgW, imgH;
        GetGraphSize(guideImage_, &imgW, &imgH);
        int x = Application::SCREEN_SIZE_X / 2 - imgW / 2;
        int y = Application::SCREEN_SIZE_Y / 2 - imgH / 2;
        DrawGraph(x, y, guideImage_, true);
        return;
    }


    DrawGraph(-100, 0, menuImage_, true);

    int mouseX, mouseY;
    GetMousePoint(&mouseX, &mouseY);
    bool isMouseDown = (GetMouseInput() & MOUSE_INPUT_LEFT);

    for (size_t i = 0; i < buttons_.size(); ++i) {
        const auto& btn = buttons_[i];
        bool isHover = (mouseX >= btn.x && mouseX <= btn.x + btn.w &&
            mouseY >= btn.y && mouseY <= btn.y + btn.h);
        bool isSelected = (static_cast<int>(i) == selectedIndex_);

        // 選択中なら枠画像を描画
        if (isSelected) {
            // 枠画像をボタンの位置・サイズに合わせて描画
            DrawGraph(btn.x - 100, btn.y - 50, selectorImage_, true);
        }

        if (isHover && isMouseDown) {
            DrawGraph(btn.x, btn.y, btn.pushedImageHandle, true);
        }
        else {
            DrawGraph(btn.x, btn.y, btn.imageHandle, true);
        }
    }
}
