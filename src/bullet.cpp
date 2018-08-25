//
// Created by Harrand on 25/08/2018.
//

#include "bullet.hpp"

Bullet::Bullet(Vector2I position, Vector2F velocity, const Texture* bullet_texture): Sprite(position, 0.0f, {Bullet::bullet_size, Bullet::bullet_size}, bullet_texture), velocity(velocity) {}

void Bullet::update(float delta)
{
    this->position_screenspace.x += static_cast<int>(std::round(velocity.x * delta * 1000.0f));
    this->position_screenspace.y += static_cast<int>(std::round(velocity.y * delta * 1000.0f));
}
