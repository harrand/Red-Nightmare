//
// Created by Harrand on 08/09/2018.
//

#include "fireball.hpp"
#include "entity_manager.hpp"

Fireball::Fireball(Vector2F position, float rotation, Vector2F scale, const Texture* fireball_texture) : Entity(position, rotation, scale, fireball_texture)
{
    this->angular_velocity = {0.0f, 0.0f, 2.0f};
}

void Fireball::update(EntityManager &manager, float delta_time)
{
    if(manager.get_mouse_listener().is_right_clicked())
        this->set_target(manager.get_mouse_listener().get_mouse_position());
    for(Entity* entity : manager.get_entities())
    {
        if(dynamic_cast<Cursor*>(entity) != nullptr || dynamic_cast<Fireball*>(entity) != nullptr)
            continue;
        if(this->get_boundary().value().intersects(entity->get_boundary().value()) && !entity->is_dead())
        {
            std::cout << "POW! Right in the kisser!\n";
            entity->remove_health(manager, 1);
            if(entity->is_dead())
                std::cout << "IT HAS DIED!\n";
        }
    }
    Entity::update(manager, delta_time);
}