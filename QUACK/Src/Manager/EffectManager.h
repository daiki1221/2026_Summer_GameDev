#pragma once
#include <vector>
#include <memory>
#include "../Object/Effect/Effect.h"

class Effect;

class EffectManager
{
public:

    void Update(float dt);

    void Draw();

    void Add(std::unique_ptr<Effect> effect);

private:

    std::vector<std::unique_ptr<Effect>> effects_;
};
