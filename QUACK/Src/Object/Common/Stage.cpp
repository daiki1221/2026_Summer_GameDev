#include <vector>
#include <map>
#include <DxLib.h>
#include "../../Utility/AsoUtility.h"
#include "../../Manager/SceneManager.h"
#include "../../Manager/ResourceManager.h"
#include "../../Object/Actor/Player.h"
#include "../../Object/Actor/Planet.h"
#include "../../Object/Common/Collider.h"
#include "../../Object/Common/Transform.h"
#include "../../Object/Actor/Duckling.h"
#include "Stage.h"

Stage::Stage(std::weak_ptr<Player> player
              , const std::vector<std::shared_ptr<Duckling>>& duckling)
	: resMng_(ResourceManager::GetInstance())
{
	player_ = player;
	for (auto& duck : duckling)
	{
		duckling_.push_back(duck);
	}
	activeName_ = NAME::MAIN_PLANET;
	step_ = 0.0f;
}

Stage::~Stage(void)
{
	// 惑星
	for (auto pair : planets_)
	{
		delete pair.second;
	}
	planets_.clear();

}

void Stage::Init(void)
{

	MakeMainStage();

	step_ = -1.0f;
	
	nestTrans_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::NEST));

	nestTrans_.scl = { 0.05f, 0.05f, 0.05f };
	MV1SetScale(nestTrans_.modelId, nestTrans_.scl);

	int materialNum2 = MV1GetMaterialNum(nestTrans_.modelId);
	for (int i = 0; i < materialNum2; ++i)
	{
		MV1SetMaterialDifColor(nestTrans_.modelId, i, { 0.0f, 0.0f, 0.0f, 1.0f });
	}

	MV1SetPosition(nestTrans_.modelId, { -100.0f, -120.0f, 300.0f }); // 地面上に設置

	skyTrans_.SetModel(resMng_.LoadModelDuplicate(
		ResourceManager::SRC::SKY_DOME));

	scl_ = SCALES;
	pos1_ = AsoUtility::VECTOR_ZERO;

	nestPos_ = { -100.0f, -120.0f, 300.0f };

	MV1SetScale(skyTrans_.modelId, scl_);
	MV1SetPosition(skyTrans_.modelId, pos1_);
	MV1SetPosition(nestTrans_.modelId, nestPos_);

}

void Stage::Update(void)
{
	// 惑星
	for (const auto& s : planets_)
	{
		s.second->Update();
	}

	skyRotY_ += 0.0005f;

	MV1SetRotationXYZ(
		skyTrans_.modelId,
		VGet(0.0f, skyRotY_, 0.0f)
	);

	MV1DrawModel(skyTrans_.modelId);


}

void Stage::Draw(void)
{
	// 惑星
	for (const auto& s : planets_)
	{
		s.second->Draw();
	}
	MV1DrawModel(nestTrans_.modelId);
	MV1DrawModel(skyTrans_.modelId);
}


void Stage::ChangeStage(NAME type)
{

	activeName_ = type;

	// 対象のステージを取得する
	activePlanet_ = GetPlanet(activeName_);

	// ステージの当たり判定をプレイヤーに設定
	player_.lock()->ClearCollider();
	player_.lock()->AddCollider(activePlanet_->GetTransform().collider_);

	// ステージの当たり判定をヒナに設定
	for (auto& duck : duckling_)
	{
		duck.lock()->ClearCollider();
		duck.lock()->AddCollider(activePlanet_->GetTransform().collider_);
	}

	step_ = TIME_STAGE_CHANGE;

}

Planet* Stage::GetPlanet(NAME type)
{
	if (planets_.count(type) == 0)
	{
		return nullPlanet;
	}

	return planets_[type];
}

void Stage::SendDucklingNest()
{
	for (auto& duck : duckling_)
	{
		if (!duck.expired())
		{
			duck.lock()->WaitNest(GetNestPos());
		}
	}
}

VECTOR Stage::GetNestPos() const
{
	return MV1GetPosition(nestTrans_.modelId);
}

void Stage::MakeMainStage(void)
{

	// 最初の惑星
	//------------------------------------------------------------------------------
	Transform planetTrans;
	planetTrans.SetModel(
		resMng_.LoadModelDuplicate(ResourceManager::SRC::MAIN_PLANET));
	planetTrans.scl = AsoUtility::VECTOR_ONE;
	planetTrans.quaRot = Quaternion();
	planetTrans.pos = { 0.0f, -100.0f, 0.0f };

	// 当たり判定(コライダ)作成
	planetTrans.MakeCollider(Collider::TYPE::STAGE);

	planetTrans.Update();

	NAME name = NAME::MAIN_PLANET;
	Planet* planet =
		new Planet(
			name, Planet::TYPE::GROUND, planetTrans);
	planet->Init();
	planets_.emplace(name, planet);
	//------------------------------------------------------------------------------

}

