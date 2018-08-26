//
// Created by Harrand on 25/08/2018.
//

#ifndef TOPAZASTEROIDS_BULLET_HPP
#define TOPAZASTEROIDS_BULLET_HPP
#include "graphics/sprite.hpp"

class Bullet : public Sprite
{
public:
    Bullet(Vector2F position, Vector2F velocity, const Texture* bullet_texture);
    virtual void update(float delta);
    static constexpr float bullet_size = 3;
private:
    Vector2F velocity;
};


#endif //TOPAZASTEROIDS_BULLET_HPP
