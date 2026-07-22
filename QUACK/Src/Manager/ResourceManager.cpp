#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Init();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Init(void)
{
	using RES = Resource;
	using RES_T = RES::TYPE;
	static std::string PATH_IMG = Application::PATH_IMAGE;
	static std::string PATH_MDL = Application::PATH_MODEL;
	static std::string PATH_EFF = Application::PATH_EFFECT;

	Resource* res;

	// プレイヤー
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/duck.mv1");
	resourcesMap_.emplace(SRC::PLAYER, res);

	// プレイヤー影
	res = new RES(RES_T::IMG, PATH_IMG + "Shadow.png");
	resourcesMap_.emplace(SRC::PLAYER_SHADOW, res);

	// メイン惑星
	res = new RES(RES_T::MODEL, PATH_MDL + "Stage/stage2.mv1");
	resourcesMap_.emplace(SRC::MAIN_PLANET, res);

	// ひなモデル
	res = new RES(RES_T::MODEL, PATH_MDL + "Player/duckling.mv1");
	resourcesMap_.emplace(SRC::DUCKLING, res);

	// 巣モデル
	res = new RES(RES_T::MODEL, PATH_MDL + "Stage/nest.mv1");
	resourcesMap_.emplace(SRC::NEST, res);

	// 空
	res = new RES(RES_T::MODEL, PATH_MDL + "SkyDome/SkyDome.mv1");
	resourcesMap_.emplace(SRC::SKY_DOME, res);

	// 敵モデル
	res = new RES(RES_T::MODEL, PATH_MDL + "Enemy/enemy.mv1");
	resourcesMap_.emplace(SRC::ENEMY, res);

	//!マーク
	res = new RES(RES_T::IMG, PATH_IMG + "Exclamation.png");
	resourcesMap_.emplace(SRC::ECXL, res);

	// 巣を目指せメッセージ
	res = new RES(RES_T::IMG, PATH_IMG + "NestGuide.png");
	resourcesMap_.emplace(SRC::GUIDE, res);

	// ダメージ時の羽
	res = new RES(RES_T::IMG, PATH_IMG + "Feather.png");
	resourcesMap_.emplace(SRC::FEATHER, res);

	// BGM
	res = new RES(RES_T::IMG, PATH_IMG + "GameBGM.mp3");
	resourcesMap_.emplace(SRC::GAME_BGM, res);

	// ひなアイコン
	res = new RES(RES_T::IMG, PATH_IMG + "Duckling.png");
	resourcesMap_.emplace(SRC::DUCKLING_IMG, res);
}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second.Release();
	}

	loadedMap_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();
	for (auto& res : resourcesMap_)
	{
		res.second->Release();
		delete res.second;
	}
	resourcesMap_.clear();
	delete instance_;
}

const Resource& ResourceManager::Load(SRC src)
{
	const Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDuplicate(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

ResourceManager::ResourceManager(void)
{
}

Resource& ResourceManager::_Load(SRC src)
{

	// ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return *resourcesMap_.find(src)->second;
	}

	// リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		// 登録されていない
		return dummy_;
	}

	// ロード処理
	rPair->second->Load();

	// 念のためコピーコンストラクタ
	loadedMap_.emplace(src, *rPair->second);

	return *rPair->second;

}
