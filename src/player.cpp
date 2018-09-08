//
// Created by Harrand on 25/08/2018.
//

#include "player.hpp"
#include "audio_manager.hpp"
#include "entity_manager.hpp"

Player::Player(Vector2F position, float rotation, Vector2F scale, const Texture *player_texture) : Entity(position, rotation, scale, player_texture){}

void Player::update(EntityManager& manager, float delta_time)
{
    this->velocity = {};
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

void Player::play_shoot_sound()
{
    AudioManager::play_shoot_sound();
}

void Player::play_struck_sound()
{
    AudioManager::play_player_struck_sound();
}