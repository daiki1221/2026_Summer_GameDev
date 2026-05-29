#pragma once
#include <memory>
#include "SceneBase.h"

class SceneManager;
class GameScene;

class ResultScene : public SceneBase
{
public:

	enum class RESULT_TYPE
	{
		OVER,	// ゲームオーバー
		CLEAR	// ゲームクリア
	};

	// コンストラクタ
	ResultScene(void);

	// デストラクタ
	~ResultScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	void SetResultType(RESULT_TYPE type) { resultType_ = type; }

private:

	RESULT_TYPE resultType_;	// リザルトの種類
	SceneManager* sceneManager_;	// シーンマネージャー
	GameScene* gameScene_;	// ゲームシーン
	int GameClear_;
	int GameClear_2;

	int GameOver_;
	int GameOver_2;
};
