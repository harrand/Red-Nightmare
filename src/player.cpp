//
// Created by Harrand on 25/08/2018.
//

#include "player.hpp"
#include "audio_manager.hpp"

Player::Player(Vector2F position, float rotation, Vector2F scale, const Texture *player_texture) : GameSprite(position, rotation, scale, {player_texture}), health(Player::default_health){}

Vector2F Player::forward() const
{
    return {-std::sin(this->get_rotation()), std::cos(this->get_rotation())};
}

void Player::update(float delta_time)
{
    GameSprite::update(delta_time);
}

std::optional<AABB> Player::get_boundary() const
{
    Vector2F half_scale = this->scale / 2.0f;
    Vector3F centre = {this->position_screenspace.x, this->position_screenspace.y, 0.0f};
    Vector3F minimum = {centre.x - half_scale.x, centre.y - half_scale.y, 0.0f};
    Vector3F maximum = {centre.x + half_scale.x, centre.y + half_scale.y, 0.0f};
    return AABB{minimum, maximum};
}

bool Player::is_dead() const
{
    return this->health == 0;
}

unsigned int Player::get_health() const
{
    return this->health;
}

void Player::set_health(unsigned int health)
{
    this->health = health;
}

void Player::add_health(unsigned int health)
{
    this->health += health;
}

void Player::remove_health(unsigned int health)
{
    if(health > this->health)
        this->health = 0;
    else
        this->health -= health;
}

void Player::play_shoot_sound()
{
    AudioManager::play_shoot_sound();
}

void Player::play_struck_sound()
{
    AudioManager::play_player_struck_sound();
}