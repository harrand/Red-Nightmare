//
// Created by Harrand on 08/09/2018.
//

#include "frostball.hpp"
#include "entity_manager.hpp"

Frostball::Frostball(Vector2F position, float rotation, Vector2F scale, const Texture* frostball_texture) : Entity(position, rotation, scale, frostball_texture, Frostball::default_health, Frostball::default_speed)
{
    this->angular_velocity = {0.0f, 0.0f, 2.0f};
}

void Frostball::update(EntityManager &manager, float delta_time)
{
    if(manager.get_mouse_listener().is_right_clicked() && manager.has_any_alive_player())
        this->set_target(manager.get_mouse_listener().get_mouse_position());
    for(Entity* entity : manager.get_entities())
    {
        if(dynamic_cast<Cursor*>(entity) != nullptr || dynamic_cast<Fireball*>(entity) != nullptr || dynamic_cast<Frostball*>(entity) != nullptr)
            continue;
        AABB normal_bound = this->get_boundary().value();
        AABB double_bound{normal_bound.get_minimum() - Vector3F{this->scale * 1.25f, 0.0f}, normal_bound.get_maximum() + Vector3F{this->scale * 1.25f, 0.0f}};
        if(double_bound.intersects(entity->get_boundary().value()) && !entity->is_dead() && dynamic_cast<Player*>(entity) == nullptr)
            entity->set_speed(entity->get_speed() * 0.95f);
        if(this->get_boundary().value().intersects(entity->get_boundary().value()) && !entity->is_dead())
            entity->remove_health(manager, 1);
    }
    Entity::update(manager, delta_time);
}