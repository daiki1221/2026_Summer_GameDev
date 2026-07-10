#include <DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "PauseScene.h"


PauseScene::PauseScene(void)
{
	isPause_ = false;
}

PauseScene::~PauseScene(void)
{
}

void PauseScene::Init(void)
{
	isPause_ = false;
	isGuide_ = false;

	pauseSelect_ = PAUSE_SELECT::RESUME;

	pauseMenuY_[0] = Application::SCREEN_SIZE_Y / 2 - 120;
	pauseMenuY_[1] = Application::SCREEN_SIZE_Y / 2;
	pauseMenuY_[2] = Application::SCREEN_SIZE_Y / 2 + 120;

	imgPauseMenu_[0] = LoadGraph("Data/Image/Menu/game.png");
	imgPauseMenu_[1] = LoadGraph("Data/Image/Menu/guide.png");
	imgPauseMenu_[2] = LoadGraph("Data/Image/Menu/title,.png");
	guideImage_ = LoadGraph("Data/Image/Menu/sousa.png");
}

void PauseScene::Update(void)
{
	auto& ins =
		InputManager::GetInstance();

	if (ins.IsTrgDown(KEY_INPUT_TAB))
	{
		isPause_ = !isPause_;

		auto camera =
			SceneManager::GetInstance().GetCamera();

		camera->SetMouseControl(isPause_);

		SetMouseDispFlag(isPause_);
	}

	if (!isPause_)
		return;

	// ëÄçÏê‡ñæï\é¶íÜ
	if (isGuide_)
	{
		if (ins.IsTrgDown(KEY_INPUT_SPACE))
		{
			isGuide_ = false;
		}

		return;
	}

		for (int i = 0; i < 3; i++)
		{
			if (IsMouseOnPauseMenu(i))
			{
				pauseSelect_ = (PAUSE_SELECT)i;
			}
		}

		static int oldMouse = 0;

		int mouse = GetMouseInput();

		bool click =
			(mouse & MOUSE_INPUT_LEFT) &&
			!(oldMouse & MOUSE_INPUT_LEFT);

		oldMouse = mouse;

		if (click)
		{
			switch (pauseSelect_)
			{

			case PAUSE_SELECT::RESUME:

				isPause_ = false;

				SceneManager::GetInstance()
					.GetCamera()
					->SetMouseControl(false);

				SetMouseDispFlag(false);

				break;


			case PAUSE_SELECT::GUIDE:

				// ëÄçÏê‡ñæÉVÅ[ÉìÇ÷
				isGuide_ = true;
				break;


			case PAUSE_SELECT::TITLE:

				SceneManager::GetInstance()
					.ChangeScene(
						SceneManager::SCENE_ID::TITLE);
				break;
			}
		}
		return;
	}

void PauseScene::Draw(void)
{
	if(!isPause_)
		return;	

	if (isGuide_)
	{
		DrawGraph(
			0,
			0,
			guideImage_,
			TRUE);

		return;
	}

	DrawPauseMenu();
}

void PauseScene::DrawPauseMenu(void)
{
	SetDrawBlendMode(
		DX_BLENDMODE_ALPHA,
		160);

	DrawBox(
		0,
		0,
		Application::SCREEN_SIZE_X,
		Application::SCREEN_SIZE_Y,
		GetColor(0, 0, 0),
		TRUE);

	SetDrawBlendMode(
		DX_BLENDMODE_NOBLEND,
		0);

	int x = Application::SCREEN_SIZE_X / 2 - 150;

	int mouseX;
	int mouseY;

	GetMousePoint(&mouseX, &mouseY);

	for (int i = 0; i < 3; i++)
	{
		int y = pauseMenuY_[i];

		bool hover =
			IsMouseOnPauseMenu(i);

		DrawGraph(
			x,
			y,
			imgPauseMenu_[i],
			TRUE);

	}
}

bool PauseScene::IsMouseOnPauseMenu(int index)
{

	int mx;
	int my;

	GetMousePoint(&mx, &my);


	int x = Application::SCREEN_SIZE_X / 2 - 150;
	int y = pauseMenuY_[index];


	return
		(
			mx >= x &&
			mx <= x + 300 &&
			my >= y &&
			my <= y + 45
			);
}

bool PauseScene::IsPause() const
{
	return isPause_;
}


