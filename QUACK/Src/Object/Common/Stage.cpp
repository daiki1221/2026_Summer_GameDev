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
#include "Stage.h"

Stage::Stage(std::weak_ptr<Player> player)
	: resMng_(ResourceManager::GetInstance())
{
	player_ = player;
	activeName_ = NAME::MAIN_PLANET;
	step_ = 0.0f;
}

Stage::~Stage(void)
{


}

void Stage::Init(void)
{

	MakeMainStage();

	step_ = -1.0f;
}

void Stage::Update(void)
{
}

void Stage::Draw(void)
{

}


void Stage::ChangeStage(NAME type)
{

	activeName_ = type;

	// 対象のステージを取得する
	activePlanet_ = GetPlanet(activeName_);

	// ステージの当たり判定をプレイヤーに設定
	player_.lock()->ClearCollider();
	player_.lock()->AddCollider(activePlanet_->GetTransform().collider_);

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

