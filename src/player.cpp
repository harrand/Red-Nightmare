//
// Created by Harrand on 25/08/2018.
//

#include "player.hpp"
#include "entity_manager.hpp"

Player::Player(Vector2F position, float rotation, Vector2F scale, const Texture *player_texture) : Entity(position, rotation, scale, player_texture), souls(0){}

void Player::update(EntityManager& manager, float delta_time)
{
    this->velocity = {};
    if(this->is_dead())
        return;
    if(manager.get_key_listener().is_key_pressed("W"))
        this->velocity += {0.0f, Player::default_speed, 0.0f};
    if(manager.get_key_listener().is_key_pressed("S"))
        this->velocity += {0.0f, -Player::default_speed, 0.0f};
    if(manager.get_key_listener().is_key_pressed("A"))
        this->velocity += {-Player::default_speed, 0.0f, 0.0f};
    if(manager.get_key_listener().is_key_pressed("D"))
        this->velocity += {Player::default_speed, 0.0f, 0.0f};
    if(this->velocity.x > 0.0f)
        this->set_animation(&manager.get_sprite_collection().get_player_right());
    else if(this->velocity.x < 0.0f)
        this->set_animation(&manager.get_sprite_collection().get_player_left());
    else if(manager.get_mouse_listener().is_left_clicked() && this->velocity.y == 0.0f)
        this->set_texture(&manager.get_sprite_collection().get_player_special());
    else
        this->set_texture(&manager.get_sprite_collection().get_player_idle());
    Entity::update(manager, delta_time);
}

void Player::on_death(EntityManager &manager)
{
    this->set_texture(&manager.get_sprite_collection().get_player_dead());
    /*
    Label& game_over_label = manager.window.emplace_child<Label>(Vector2I{manager.window.get_width() / 2, manager.window.get_height() / 2}, manager.default_font, Vector3F{1.0f, 0.0f, 0.0f}, "Game Over!");
    game_over_label.set_local_position_pixel_space(game_over_label.get_local_position_pixel_space() - Vector2I{game_over_label.get_width() / 2, game_over_label.get_height() / 2});
     */
}

unsigned int Player::get_souls() const
{
    return this->souls;
}

void Player::set_souls(unsigned int souls)
{
    this->souls = souls;
}

void Player::add_souls(unsigned int souls)
{
    this->souls += souls;
}

void Player::remove_souls(unsigned int souls)
{
    if(souls > this->souls)
        this->souls = 0;
    else
        this->souls -= souls;
}