#pragma once

#include <DxLib.h>

class FeatherParticle
{
public:

    FeatherParticle();
	void Init(const VECTOR& pos);
    void Update();
    void Draw();

    bool IsDead() const { return life_ <= 0.0f; }

private:

    VECTOR pos_;
    VECTOR vel_;

    float life_;
    float rot_;
    float rotSpeed_;

    int img_;
};
