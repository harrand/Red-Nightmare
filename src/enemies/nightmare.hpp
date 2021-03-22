//
// Created by Harrand on 05/02/2019.
//

#ifndef REDNIGHTMARE_NIGHTMARE_HPP
#define REDNIGHTMARE_NIGHTMARE_HPP
#include "entity.hpp"

class Nightmare : public Entity
{
public:
    Nightmare(Vector2F position, float rotation, Vector2F scale, const Texture* nightmare_texture);
    virtual void update(EntityManager& manager, float delta_time) override;
    virtual void on_death(EntityManager& manager) override;
    virtual void remove_health(EntityManager& manager, unsigned int health);
    bool is_held() const;
    void set_held_by_cursor(bool held);
    void set_held_by_orb(bool held);
    static constexpr unsigned int default_health = 10000;
    static constexpr float default_speed = 0.25f;
private:
    bool held_by_cursor;
    bool held_by_orb;
};


#endif //REDNIGHTMARE_NIGHTMARE_HPP
