#include <DxLib.h>
#include <ctime>
#include "Application.h"

// WinMain関数
//---------------------------------
int WINAPI WinMain(
	_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

	// 乱数の初期化
	srand((unsigned int)time(nullptr));

	// インスタンスの生成
	Application::CreateInstance();

	// インスタンスの取得
	Application& instance = Application::GetInstance();

	if (instance.IsInitFail())
	{
		// 初期化失敗
		return -1;
	}

	int bgm_ = LoadSoundMem("Data/Sound/TitleBGM.wav");
	PlaySoundMem(bgm_, DX_PLAYTYPE_LOOP, TRUE);

	// 実行
	instance.Run();

	StopSoundMem(bgm_);
	DeleteSoundMem(bgm_);

	// 解放
	instance.Destroy();

	if (instance.IsReleaseFail())
	{
		// 解放失敗
		return -1;
	}

	return 0;

}
