#pragma once
#include <memory>
#include <vector>
#include "SceneBase.h"
#include "ResultScene.h"
class Stage;
class Player;
class Duckling;
class Enemy;
class Bullet;
class PauseScene;

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

	std::vector<std::shared_ptr<Bullet>> bullet_;

	std::unique_ptr<PauseScene> pauseScene_;

	float time_; //残り時間

	int pattern;

	std::vector<std::vector<VECTOR>> patterns =
	{
		{
			{-1500,-96, 2280},
			{-1000,-13,-3500},
			{ 3800,-60,-2360}
		},
		{
			{   76,  34, 2155},
			{-3743,-186, 3952},
			{  960,-106,-3536}
		},
		{
			{   76, 34, 2155},
			{  300,-30,  800},
			{ 3800,-60,-2360}
		}
	};

	bool isNestMessage_;
	bool isNestMessageDisplayed_;
	float nestMessageTimer_;
	
	int imgNestMessage_;

	int bgmHandle_;

	
};
