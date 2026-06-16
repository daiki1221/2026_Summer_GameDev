#pragma once
#include <memory>
#include <vector>
#include "SceneBase.h"
#include "ResultScene.h"
class Stage;
class Player;
class Duckling;
class Enemy;

class GameScene : public SceneBase
{

public:
	
	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	

private:

	// ステージ
	std::unique_ptr<Stage> stage_;

	std::shared_ptr<Player> player_;

	std::vector<std::shared_ptr<Duckling>> duckling_;

	std::unique_ptr<Enemy> enemy_;

	float time_; //残り時間

	
};
