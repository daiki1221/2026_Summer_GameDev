#pragma once

#include <DxLib.h>

class Bullet
{
public:

    float radius_;

    Bullet(
        const VECTOR& pos,
        const VECTOR& dir);

    void Update();
    void Draw();

    bool IsDead() const { return isDead_; }
    void SetDead() { isDead_ = true; }
    const VECTOR& GetPos() const { return pos_; }
    float GetRadius() const { return radius_; }

private:

    VECTOR pos_;
    VECTOR dir_;

    float speed_;
    float lifeTime_;

    bool isDead_;

};