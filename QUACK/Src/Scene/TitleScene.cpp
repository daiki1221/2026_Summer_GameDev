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

	image_ = LoadGraph("Data/Image/Title/Title.png");

	select_ = SELECT::START;

	menuY_[0] = Application::SCREEN_SIZE_Y / 2-40;
	menuY_[1] = Application::SCREEN_SIZE_Y / 2 + 60;
	menuY_[2] = Application::SCREEN_SIZE_Y / 2 + 160;

	startImg_ = LoadGraph("Data/Image/Title/Start.png");
	guideImg_ = LoadGraph("Data/Image/Title/Guide.png");
	endImg_ = LoadGraph("Data/Image/Title/End.png");

}

void TitleScene::Update(void)
{

	auto& ins = InputManager::GetInstance();

	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState =
		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	VECTOR stick = ins.GetDirectionXZAKey(
		padState.AKeyLX,
		padState.AKeyLY);

	bool stickUp = stick.z > 0.7f;
	bool stickDown = stick.z < -0.7f;

	if (GetJoypadNum() == 0) {

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
		if (GetMouseInput() & MOUSE_INPUT_LEFT ||
			ins.IsTrgDown(KEY_INPUT_SPACE))
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
	else
	{

		// 上選択
		if (stickUp && !stickUpOld_)
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
		if (stickDown && !stickDownOld_)
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
		if (ins.IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::RIGHT))
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
		stickUpOld_ = stickUp;
		stickDownOld_ = stickDown;
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


	int mouseX;
	int mouseY;

	GetMousePoint(&mouseX, &mouseY);


	for (int i = 0; i < 3; i++)
	{

		int x = Application::SCREEN_SIZE_X / 2 - 190;
		int y = menuY_[i];

		int w = 350;
		int h = 85;


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

		// ボタン
		DrawGraph(
			x + 10,
			y + 5,
			(i == 0) ? startImg_ : (i == 1) ? guideImg_ : endImg_,
			true);

		float scale = (selected || mouseOn) ? 1.1f : 1.0f;

		DrawRotaGraph(
			x + 185,
			y + 45,
			scale,
			0.0,
			(i == 0) ? startImg_ :
			(i == 1) ? guideImg_ :
			endImg_,
			TRUE);

	}
}

bool TitleScene::IsMouseOnMenu(int index)
{
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	int x = Application::SCREEN_SIZE_X / 2 - 190 + 10;
	int y = menuY_[index] + 5;

	int w, h;
	GetGraphSize(startImg_, &w, &h);

	return (mouseX >= x &&
		mouseX <= x + w &&
		mouseY >= y &&
		mouseY <= y + h);

}

