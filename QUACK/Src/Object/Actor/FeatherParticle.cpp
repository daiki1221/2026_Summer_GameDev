#include "FeatherParticle.h"
#include "../../Manager/ResourceManager.h"
#include <cstdlib>

FeatherParticle::FeatherParticle()
{
    pos_ = VGet(0, 0, 0);
    vel_ = VGet(0, 0, 0);

    life_ = 1.0f;
    rot_ = 0.0f;
    rotSpeed_ = 0.0f;

    img_ = -1;
}

void FeatherParticle::Init(const VECTOR& pos)
{
    pos_ = pos;

    vel_ = VGet(
        (rand() % 200 - 100) * 0.01f,
        (rand() % 200) * 0.02f,
        (rand() % 200 - 100) * 0.01f
    );

    rotSpeed_ = (rand() % 200 - 100) * 0.01f;
    life_ = 1.0f;
    rot_ = 0.0f;

    // 画像は外からセットでもOK
	img_ = ResourceManager::GetInstance().Load(ResourceManager::SRC::FEATHER).handleId_;;
}

void FeatherParticle::Update()
{
    float t = life_;
    int alpha = (int)(255 * t * t);

    // 重力
    vel_ = VAdd(vel_, VGet(
        (rand() % 100 - 50) * 0.0005f,
        0.0f,
        (rand() % 100 - 50) * 0.0005f
    ));
    vel_.x += sinf(rot_) * 0.01f;
    vel_.y -= 0.02f;

    pos_ = VAdd(pos_, vel_);

    rot_ += rotSpeed_;

    life_ -= 1.0f / 60.0f;


}

void FeatherParticle::Draw()
{
    if (img_ < 0) return;

    int alpha = (int)(life_ * 255);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

    DrawBillboard3D(
        pos_,
        0.5f,
        0.5f,
        10.0f,   // 羽のサイズ
        rot_,
        img_,
        TRUE
    );

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}