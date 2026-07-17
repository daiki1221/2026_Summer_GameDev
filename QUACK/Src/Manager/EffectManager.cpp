#include "EffectManager.h"
#include <algorithm> 
#include <DxLib.h>

void EffectManager::Update(float dt)
{
    for (auto& e : effects_)
        e->Update(dt);

    effects_.erase(
        std::remove_if(
            effects_.begin(),
            effects_.end(),
            [](const auto& e)
            {
                return !e->IsAlive();
            }),
        effects_.end());
}

void EffectManager::Draw()
{
    for (auto& e : effects_)
        e->Draw();

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void EffectManager::Add(std::unique_ptr<Effect> effect)
{
    effects_.push_back(std::move(effect));
}