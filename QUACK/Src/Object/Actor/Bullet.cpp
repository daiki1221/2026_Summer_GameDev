#include "Bullet.h"

Bullet::Bullet(
    const VECTOR& pos,
    const VECTOR& dir)
{
    pos_ = pos;
    dir_ = dir;

    speed_ = 15.0f;
    lifeTime_ = 3.0f;

	radius_ = 50.0f;

    isDead_ = false;
}

void Bullet::Update()
{
    pos_ = VAdd(
        pos_,
        VScale(dir_, speed_));

    lifeTime_ -= 1.0f / 60.0f;

    if (lifeTime_ <= 0.0f)
    {
        isDead_ = true;
    }
}

void Bullet::Draw()
{
    DrawSphere3D(
        pos_,
        30.0f,
        16,
        GetColor(255, 255, 0),
        GetColor(255, 255, 0),
        TRUE);
}