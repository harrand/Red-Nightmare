//
// Created by Harrand on 08/09/2018.
//

#ifndef REDNIGHTMARE_FIREBALL_HPP
#define REDNIGHTMARE_FIREBALL_HPP
#include "entity.hpp"

class Fireball : public Entity
{
public:
    Fireball(Vector2F position, float rotation, Vector2F scale, const Texture* fireball_texture);
    virtual void update(EntityManager& manager, float delta_time) override;
};


#endif //REDNIGHTMARE_FIREBALL_HPP
