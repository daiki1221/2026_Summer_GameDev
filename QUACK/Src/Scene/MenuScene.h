#pragma once
#include <vector>
#include "../Scene/SceneBase.h"

class InputManager;
class SceneManager;
class GameScene;
class MenuScene : public SceneBase
{
public:
	// コンストラクタ
	MenuScene(void);

	// デストラクタ
	~MenuScene(void) override;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;

	GameScene* gameScene_;	// ゲームシーン

private:
	int menuImage_;
	int menuImagePushed_;
	int selectorImage_ = -1; // 選択中枠画像のハンドル
	bool isGuideOpen_ = false;	// メニューが開いているかどうか
	int selectedIndex_ = 0; // 現在選択中のボタンint sele
	struct Button {
		enum TYPE { TITLE, GUIDE, EXIT };
		int x, y, w, h;
		TYPE type;
		int imageHandle;        // 通常画像
		int pushedImageHandle;  // pushed画像
	};
	std::vector<Button> buttons_;
	InputManager* inputManager_;
	SceneManager* sceneManager_;

	int guideImage_[2];

	bool isSecondPage_ = false;


};