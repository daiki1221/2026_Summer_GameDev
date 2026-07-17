#include "WindEffect.h"

WindEffect::WindEffect(const VECTOR& pos, const VECTOR& dir)
{
    pos_ = pos;
    dir_ = dir;

    length_ = 20.0f + rand() % 30;
    alpha_ = 255.0f;
}

void WindEffect::Update(float dt)
{
    pos_ = VAdd(pos_, VScale(dir_, 20.0f));

    length_ = 50.0f + rand() % 50;

    alpha_ -= 300.0f * dt;

    if (alpha_ <= 0.0f)
    {
        alive_ = false;
    }
}

void WindEffect::Draw()
{
    VECTOR end = VAdd(pos_, VScale(dir_, length_));
    VECTOR right = VGet(dir_.z, 0.0f, -dir_.x);

    SetDrawBlendMode(
        DX_BLENDMODE_ALPHA,
        (int)alpha_);

    DrawLine3D(
        VAdd(pos_, VScale(right, 5)),
        VAdd(end, VScale(right, 5)),
        GetColor(255, 255, 255));

    DrawLine3D(
        pos_,
        end,
        GetColor(255, 255, 255));

    DrawLine3D(
        VAdd(pos_, VScale(right, -5)),
        VAdd(end, VScale(right, -5)),
        GetColor(255, 255, 255));

    SetDrawBlendMode(
        DX_BLENDMODE_NOBLEND,
        0);
}