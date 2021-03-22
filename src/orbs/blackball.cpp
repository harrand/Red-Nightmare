//
// Created by Harrand on 08/09/2018.
//

#include "blackball.hpp"
#include "entity_manager.hpp"

Blackball::Blackball(Vector2F position, float rotation, Vector2F scale, const Texture* fireball_texture) : Entity(position, rotation, scale, fireball_texture, Blackball::default_health, Blackball::default_speed)
{
    this->angular_velocity = {0.0f, 0.0f, 0.1f};
}

void Blackball::update(EntityManager &manager, float delta_time)
{
    this->speed = Blackball::default_speed / this->scale.length();
    if(manager.get_mouse_listener().is_right_clicked() && manager.has_any_alive_player())
        this->set_target(manager.get_mouse_listener().get_mouse_position());
    for(Entity* entity : manager.get_entities())
    {
        if(dynamic_cast<Cursor*>(entity) != nullptr || dynamic_cast<Fireball*>(entity) != nullptr || dynamic_cast<Blackball*>(entity) != nullptr || dynamic_cast<Frostball*>(entity) != nullptr)
            continue;
        Player* player_component = dynamic_cast<Player*>(entity);
        if(player_component == nullptr || player_component->is_dead())
        {
            entity->clear_forces();
            Vector2F displacement = this->position_screenspace - entity->position_screenspace;
            entity->add_force(Vector3F{displacement.normalised(), 0.0f} / std::sqrt(displacement.length() + 1) * 5.0f *
                              std::pow(this->scale.length(), 1.5f));
        }
        if(this->get_boundary().value().intersects(entity->get_boundary().value()) && !entity->is_dead())
        {
            entity->remove_health(manager, static_cast<unsigned int>(1 + this->scale.length() * 0.005f));
            if(entity->is_dead())
            {
                if(dynamic_cast<Nightmare*>(entity) != nullptr)
                {
                    this->scale *= 2.0f;
                    this->angular_velocity *= 2.0f;
                }
                else
                {
                    this->scale *= 1.025f;
                    this->angular_velocity *= 1.025f;
                }
            }
        }
    }
    Entity::update(manager, delta_time);
}