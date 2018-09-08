//
// Created by Harrand on 08/09/2018.
//

#ifndef REDNIGHTMARE_CURSOR_HPP
#define REDNIGHTMARE_CURSOR_HPP
#include "entity.hpp"

class Cursor : public Entity
{
public:
    Cursor(const Texture* cursor_texture);
    virtual void update(EntityManager& manager, float delta_time) override;
    bool is_activated() const;
    static constexpr float CURSOR_SIZE = 35.0f;
private:
    bool active;
    std::unordered_map<Entity*, Vector2F> held_entities;
};


#endif //REDNIGHTMARE_CURSOR_HPP
