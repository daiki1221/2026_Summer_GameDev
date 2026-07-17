#pragma once
#include <DxLib.h>

class Effect
{
public:
    virtual ~Effect() {}

    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;

    bool IsAlive() const { return alive_; }

protected:
    bool alive_ = true;
};