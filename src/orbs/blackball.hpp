//
// Created by Harrand on 08/09/2018.
//

#ifndef REDNIGHTMARE_BLACKBALL_HPP
#define REDNIGHTMARE_BLACKBALL_HPP

#include "entity.hpp"

class Blackball : public Entity
{
public:
    Blackball(Vector2F position, float rotation, Vector2F scale, const Texture* fireball_texture);
    virtual void update(EntityManager& manager, float delta_time) override;
    static constexpr unsigned int default_health = 1;
    static constexpr float default_speed = 10.0f;
};


#endif //REDNIGHTMARE_BLACKBALL_HPP
