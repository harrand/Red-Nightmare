//
// Created by Harrand on 07/09/2018.
//

#ifndef REDNIGHTMARE_GHOST_HPP
#define REDNIGHTMARE_GHOST_HPP
#include "player.hpp"

class Ghost : public Entity
{
public:
    Ghost(Vector2F position, float rotation, Vector2F scale, const Texture* ghost_texture);
    virtual void update(EntityManager& manager, float delta_time) override;
    virtual void on_death(EntityManager& manager) override;
    static constexpr unsigned int default_health = 1;
    static constexpr float default_speed = 50.0f;
};


#endif //REDNIGHTMARE_GHOST_HPP
