//
// Created by Harrand on 08/09/2018.
//

#include "cursor.hpp"
#include "entity_manager.hpp"

Cursor::Cursor(const Texture* cursor_texture) : Entity({}, 0.0f, {Cursor::CURSOR_SIZE, Cursor::CURSOR_SIZE}, cursor_texture), active(false){}

void Cursor::update(EntityManager& manager, float delta_time)
{
    this->position_screenspace = manager.get_mouse_listener().get_mouse_position();
    Player* closest = this->get_closest_player(manager);
    if(manager.get_mouse_listener().is_left_clicked() && closest != nullptr && !closest->is_moving())
    {
        this->set_texture(&manager.get_sprite_collection().get_on_rune());
        this->active = true;
    }
    else
    {
        this->set_texture(&manager.get_sprite_collection().get_off_rune());
        this->active = false;
    }
    Entity::update(manager, delta_time);
}

bool Cursor::is_activated() const
{
    return this->active;
}