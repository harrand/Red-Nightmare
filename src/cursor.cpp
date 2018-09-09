//
// Created by Harrand on 08/09/2018.
//

#include "cursor.hpp"
#include "entity_manager.hpp"

Cursor::Cursor(const Texture* cursor_texture) : Entity({}, 0.0f, {Cursor::CURSOR_SIZE, Cursor::CURSOR_SIZE}, cursor_texture), active(false), held_entities()
{
    this->angular_velocity = {0.0f, 0.0f, 1.0f};
}

void Cursor::update(EntityManager& manager, float delta_time)
{
    this->position_screenspace = manager.get_mouse_listener().get_mouse_position();
    Player* closest = this->get_closest_player(manager);
    if(manager.get_mouse_listener().is_left_clicked() && closest != nullptr && (!closest->is_moving() || closest->net_force().length() > 0.0f) && manager.has_any_alive_player())
    {
        this->set_texture(&manager.get_sprite_collection().get_on_rune());
        this->active = true;
    }
    else
    {
        this->set_texture(&manager.get_sprite_collection().get_off_rune());
        this->active = false;
    }
    for(Ghost* ghost : manager.get_ghosts())
    {
        if(ghost->get_boundary().has_value() && ghost->get_boundary().value().intersects({this->position_screenspace, 0.0f}) && this->is_activated() && !ghost->is_dead() && manager.has_any_alive_player())
        {
            ghost->set_kinematic(true);
            this->held_entities.emplace(ghost, ghost->position_screenspace - this->position_screenspace);
        }
        else
        {
            ghost->set_kinematic(false);
            this->held_entities.erase(ghost);
        }
    }
    for(const auto& [entity, offset] : this->held_entities)
    {
        entity->position_screenspace = this->position_screenspace + offset;
    }
    Entity::update(manager, delta_time);
}

bool Cursor::is_activated() const
{
    return this->active;
}