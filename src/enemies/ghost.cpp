//
// Created by Harrand on 07/09/2018.
//

#include "ghost.hpp"
#include "entity_manager.hpp"

Ghost::Ghost(Vector2F position, float rotation, Vector2F scale, const Texture* ghost_texture) : Entity(position, rotation, scale, ghost_texture)
{
    this->health = Ghost::default_health;
}

void Ghost::update(EntityManager& manager, float delta_time)
{
    Player* closest = this->get_closest_player(manager);
    if(closest != nullptr)
    {
        this->set_target(closest->position_screenspace);
        if(this->get_boundary().value().intersects(Vector3F{closest->position_screenspace, 0.0f}))
        {
            if(this->is_dead())
            {
                closest->add_health(manager, 10);
                closest->add_souls(1);
                manager.remove_sprite(*this);
            }
            else
                closest->remove_health(manager, 1);
        }
    }
    if(this->is_dead())
        return;
    if(this->has_target())
    {
        Vector2F to_target = this->target.value() - this->position_screenspace;
        if (to_target.x > 0.0f)
            this->set_animation(&manager.get_sprite_collection().get_ghost_right());
        else if (to_target.x < 0.0f)
            this->set_animation(&manager.get_sprite_collection().get_ghost_left());
        else
            this->set_texture(&manager.get_sprite_collection().get_ghost_idle());
    }
    else
    {
        if (this->velocity.x > 0.0f)
            this->set_animation(&manager.get_sprite_collection().get_ghost_right());
        else if (this->velocity.x < 0.0f)
            this->set_animation(&manager.get_sprite_collection().get_ghost_left());
        else
            this->set_texture(&manager.get_sprite_collection().get_ghost_idle());
    }
    Entity::update(manager, delta_time);
}

void Ghost::on_death(EntityManager& manager)
{
    this->set_texture(&manager.get_sprite_collection().get_ghost_dead());
    this->velocity = {};
}