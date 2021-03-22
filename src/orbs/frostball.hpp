//
// Created by Harrand on 08/09/2018.
//

#ifndef REDNIGHTMARE_FROSTBALL_HPP
#define REDNIGHTMARE_FROSTBALL_HPP
#include "entity.hpp"

class Frostball : public Entity
{
public:
    Frostball(Vector2F position, float rotation, Vector2F scale, const Texture* frostball_texture);
    virtual void update(EntityManager& manager, float delta_time) override;
    static constexpr unsigned int default_health = 1;
    static constexpr float default_speed = 0.1f;
};


#endif //REDNIGHTMARE_FROSTBALL_HPP
