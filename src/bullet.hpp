//
// Created by Harrand on 25/08/2018.
//

#ifndef TOPAZASTEROIDS_BULLET_HPP
#define TOPAZASTEROIDS_BULLET_HPP
#include "asteroid.hpp"

class Player;
class EntityManager;

class Bullet : public DynamicSprite
{
public:
    Bullet(Vector2F position, Vector2F velocity, const Texture* bullet_texture, EntityManager& manager, DynamicSprite* shooter = nullptr);
    virtual void on_collision(Asteroid& asteroid);
    virtual void on_collision(Player& player);
    static constexpr float base_bullet_speed_multiplier = 250.0f;
    static constexpr float bullet_size = 3;
private:
    virtual void on_collision(PhysicsObject& other) override;
    EntityManager& manager;
    DynamicSprite* shooter;
};


#endif //TOPAZASTEROIDS_BULLET_HPP
