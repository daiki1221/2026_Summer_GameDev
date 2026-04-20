#include <DxLib.h>
#include "../../Application.h"
#include "Stage.h"

Stage::Stage(void)
{
}

Stage::~Stage(void)
{
}

void Stage::Init(void)
{

	modelId_ = MV1LoadModel(
		(Application::PATH_MODEL + "Stage/stage.mv1").c_str());
}

void Stage::Update(void)
{
}

void Stage::Draw(void)
{
	MV1DrawModel(modelId_);
}

void Stage::Release(void)
{
	MV1DeleteModel(modelId_);
}

