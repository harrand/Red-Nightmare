//
// Created by Harrand on 05/02/2019.
//

#include "nightmare.hpp"
#include "entity_manager.hpp"

Nightmare::Nightmare(Vector2F position, float rotation, Vector2F scale, const Texture* ghost_texture) : Entity(position, rotation, scale, ghost_texture), held_by_cursor(false), held_by_orb(false)
{
    this->health = Nightmare::default_health;
}

void Nightmare::update(EntityManager& manager, float delta_time)
{
    // Ensure nightmare's speed always recovers.
    if(this->is_held())
        this->speed = 0.0f;
    else if(this->held_by_cursor || this->held_by_orb)
        this->speed = Nightmare::default_speed * 0.5f;
    else if(this->speed < Nightmare::default_speed)
    {
        this->speed += Nightmare::default_speed * 0.001f;
        this->speed *= 1.1f;
    }
    Player* closest = this->get_closest_player(manager);
    if(closest != nullptr)
    {
        this->set_target(closest->position_screenspace);
        if(this->get_boundary().value().intersects(Vector3F{closest->position_screenspace, 0.0f}))
        {
            if(this->is_dead())
            {
                closest->add_health(manager, 1000);
                closest->add_souls(10);
                manager.remove_sprite(*this);
            }
            else if(manager.has_any_alive_player())
                this->remove_health(manager, 1);
        }
    }
    if(this->is_dead())
        return;
    if(this->has_target())
    {
        Vector2F to_target = this->target.value() - this->position_screenspace;
        if (to_target.x > 0.0f)
            this->set_animation(&manager.get_sprite_collection().get_nightmare_right());
        else if (to_target.x < 0.0f)
            this->set_animation(&manager.get_sprite_collection().get_nightmare_left());
        else
            this->set_texture(&manager.get_sprite_collection().get_nightmare_idle());
    }
    else
    {
        if (this->velocity.x > 0.0f)
            this->set_animation(&manager.get_sprite_collection().get_nightmare_right());
        else if (this->velocity.x < 0.0f)
            this->set_animation(&manager.get_sprite_collection().get_nightmare_left());
        else
            this->set_texture(&manager.get_sprite_collection().get_nightmare_idle());
    }
    Entity::update(manager, delta_time);
}

void Nightmare::on_death(EntityManager& manager)
{
    this->set_texture(&manager.get_sprite_collection().get_nightmare_dead());
    this->velocity = {};
}

void Nightmare::remove_health(EntityManager& manager, unsigned int health)
{
    // teleport nightmare to random spot on the screen, hurt the player for 2x the damage.
    for(Player* player : manager.get_players())
        player->remove_health(manager, health * 2);
    if(manager.get_screen_wrapping_bounds().has_value())
    {
        Vector2I bounds = manager.get_screen_wrapping_bounds().value();
        Random rand;
        this->position_screenspace = {rand.next_float(0.0f, bounds.x), rand.next_float(0.0f, bounds.y)};
    }
    Entity::remove_health(manager, health);
}

bool Nightmare::is_held() const
{
    return this->held_by_cursor && this->held_by_orb;
}

void Nightmare::set_held_by_cursor(bool held)
{
    this->held_by_cursor = held;
}

void Nightmare::set_held_by_orb(bool held)
{
    this->held_by_orb = held;
}