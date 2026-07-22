#pragma once
#include "SceneBase.h"

class TitleScene : public SceneBase
{

public:

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	

private:

	int image_;

	enum class SELECT
	{
		START,
		GUIDE,
		END,
	};

	SELECT select_;

	int menuY_[3];

	void DrawMenu(void);

	bool IsMouseOnMenu(int index);

	int startImg_;
	int guideImg_;
	int endImg_;
};
