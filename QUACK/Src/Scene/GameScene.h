#pragma once
#include <memory>
#include "SceneBase.h"
class Stage;
class Player;

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
};
