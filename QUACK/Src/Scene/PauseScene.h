#pragma once
#include <memory>
#include <vector>
#include "SceneBase.h"
#include "ResultScene.h"
#include "../Common/Fader.h"

class PauseScene : public SceneBase
{

public:

	enum class PAUSE_SELECT
	{
		RESUME,
		GUIDE,
		TITLE,
	};

	// コンストラクタ
	PauseScene(void);

	// デストラクタ
	~PauseScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	void DrawPauseMenu(void);

	bool IsMouseOnPauseMenu(int index);

	bool IsPause() const;

private:

	bool isPause_;

	PAUSE_SELECT pauseSelect_;

	int pauseMenuY_[3];

	int imgPauseMenu_[3];

	bool isGuide_;
	int guideImage_;
	Fader* fade_;

};
