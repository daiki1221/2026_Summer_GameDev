#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Application.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	:
	SceneBase()
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	
	sceMng_.GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);

	image_ = LoadGraph("Data/Image/Title.png");

	select_ = SELECT::START;

	menuY_[0] = Application::SCREEN_SIZE_Y / 2;
	menuY_[1] = Application::SCREEN_SIZE_Y / 2 + 60;
	menuY_[2] = Application::SCREEN_SIZE_Y / 2 + 120;
}

void TitleScene::Update(void)
{

	auto const& ins = InputManager::GetInstance();

	// マウス位置取得
	int mouseX;
	int mouseY;

	GetMousePoint(&mouseX, &mouseY);


	// マウスで選択
	for (int i = 0; i < 3; i++)
	{
		if (IsMouseOnMenu(i))
		{
			select_ = (SELECT)i;
		}
	}


	// クリック決定
	if (GetMouseInput() & MOUSE_INPUT_LEFT)
	{
		switch (select_)
		{
		case SELECT::START:

			sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);

			break;


		case SELECT::GUIDE:

			// 操作説明シーンへ
			sceMng_.OpenMenu();

			break;


		case SELECT::END:

			DxLib_End();

			break;
		}
	}

	// 上選択
	if (ins.IsTrgDown(KEY_INPUT_UP))
	{
		int num = (int)select_;

		num--;

		if (num < 0)
		{
			num = 2;
		}

		select_ = (SELECT)num;
	}


	// 下選択
	if (ins.IsTrgDown(KEY_INPUT_DOWN))
	{
		int num = (int)select_;

		num++;

		if (num > 2)
		{
			num = 0;
		}

		select_ = (SELECT)num;
	}


	// 決定
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		switch (select_)
		{
		case SELECT::START:

			sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);

			break;


		case SELECT::GUIDE:

			// 操作説明シーンへ
			sceMng_.OpenMenu();

			break;


		case SELECT::END:

			DxLib_End();
			break;
		}
	}

}

void TitleScene::Draw(void)
{
	DrawRotaGraph(
		Application::SCREEN_SIZE_X / 2,
		Application::SCREEN_SIZE_Y / 2,
		1.0f, 0.0, image_, true);

	DrawMenu();
}

void TitleScene::DrawMenu(void)
{
	const char* menu[3] =
	{
		"ゲームスタート",
		"操作説明",
		"ゲームをおわる"
	};


	int mouseX;
	int mouseY;

	GetMousePoint(&mouseX, &mouseY);


	for (int i = 0; i < 3; i++)
	{

		int x = Application::SCREEN_SIZE_X / 2 - 150;
		int y = menuY_[i];

		int w = 300;
		int h = 45;


		bool mouseOn =
			(mouseX >= x &&
				mouseX <= x + w &&
				mouseY >= y &&
				mouseY <= y + h);


		bool selected =
			((int)select_ == i);


		// ボタン色
		int boxColor;

		if (mouseOn || selected)
		{
			// 選択中
			boxColor = GetColor(255, 200, 0);
		}
		else
		{
			boxColor = GetColor(80, 80, 80);
		}


		// ボタン背景
		DrawBox(
			x,
			y,
			x + w,
			y + h,
			boxColor,
			TRUE);


		// 枠
		DrawBox(
			x,
			y,
			x + w,
			y + h,
			GetColor(255, 255, 255),
			FALSE);


		// 文字
		int textColor;

		if (mouseOn || selected)
		{
			textColor = GetColor(255, 255, 255);
		}
		else
		{
			textColor = GetColor(200, 200, 200);
		}


		DrawFormatString(
			x + 80,
			y + 12,
			textColor,
			menu[i]);
	}
}

bool TitleScene::IsMouseOnMenu(int index)
{
	int mouseX;
	int mouseY;

	GetMousePoint(&mouseX, &mouseY);


	int x = Application::SCREEN_SIZE_X / 2 - 100;
	int y = menuY_[index];


	if (mouseX >= x &&
		mouseX <= x + 250 &&
		mouseY >= y - 10 &&
		mouseY <= y + 40)
	{
		return true;
	}


	return false;
}

