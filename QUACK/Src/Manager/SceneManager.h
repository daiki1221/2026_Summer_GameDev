#pragma once
#include <vector>
#include <chrono>

class SceneBase;
class Fader;
class Camera;
class MiniCamera;
class Food;
class Player;

class SceneManager
{

public:

	// 背景色
	static constexpr int BACKGROUND_COLOR_R = 0;
	static constexpr int BACKGROUND_COLOR_G = 139;
	static constexpr int BACKGROUND_COLOR_B = 139;

	// ディレクショナルライトの方向
	static constexpr VECTOR LIGHT_DIRECTION = { 0.3f, -0.7f, 0.8f };

	// シーン管理用
	enum class SCENE_ID
	{
		NONE,
		TITLE,
		GAME,
		MENU,
		RESULT,
	};

	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static SceneManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 3Dの初期化
	void Init3D(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// リソースの破棄
	void Destroy(void);

	// 状態遷移
	void ChangeScene(SCENE_ID nextId);

	// シーンIDの取得
	SCENE_ID GetSceneID(void);

	// デルタタイムの取得
	float GetDeltaTime(void) const;

	// カメラの取得
	Camera* GetCamera(void) const;

	const std::vector<Food*>& GetFoodList() const;

	// 巣に到達したときの処理
	void OnReachNest(void);

	void GameOver(void);

	void OpenMenu(); // メニューを開く
	void ReturnPrevScene(); // 直前のシーンに戻る
	SCENE_ID prevSceneType_;                      // 直前のシーン
	bool skipReset_;

	

private:



	SCENE_ID sceneType_;					// 現在のシーン
	SCENE_ID nextSceneType_;				// 次のシーン

	std::vector<Food*> foodList_;

	// 静的インスタンス
	static SceneManager* instance_;

	SCENE_ID sceneId_;
	SCENE_ID waitSceneId_;

	// フェード
	Fader* fader_;

	// 各種シーン
	SceneBase* scene_;

	// カメラ
	Camera* camera_;

	// ミニカメラ
	MiniCamera* miniCamera_;

	Food* food_;

	Player* player_;

	// シーン遷移中判定
	bool isSceneChanging_;


	// デルタタイム
	std::chrono::system_clock::time_point preTime_;
	float deltaTime_;


	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);

	// コピーコンストラクタも同様
	SceneManager(const SceneManager& instance) = default;

	// デストラクタも同様
	~SceneManager(void) = default;

	// デルタタイムをリセットする
	void ResetDeltaTime(void);

	// シーン遷移
	void DoChangeScene(SCENE_ID sceneId);

	// フェード
	void Fade(void);

};