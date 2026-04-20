#pragma once
class Stage
{
public:

	// コンストラクタ
	Stage(void);

	// デストラクタ
	~Stage(void);

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// 解放
	void Release(void);
private:

	// ステージモデルのハンドルID
	int modelId_;

};

