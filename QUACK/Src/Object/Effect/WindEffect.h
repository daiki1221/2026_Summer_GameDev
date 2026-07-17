#pragma once

#include <DxLib.h>
#include "Effect.h"

class WindEffect : public Effect
{
public:
    WindEffect(const VECTOR& pos, const VECTOR& dir);

    void Update(float dt) override;
    void Draw() override;

private:
    VECTOR pos_;
    VECTOR dir_;

    float length_;
    float alpha_;
};