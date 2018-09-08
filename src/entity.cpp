//
// Created by Harrand on 07/09/2018.
//

#include "player.hpp"
#include "entity_manager.hpp"

Entity::Entity(Vector2F position, float rotation, Vector2F scale, const Texture* texture) : GameSprite(position, rotation, scale, texture), health(Entity::default_health), target(std::nullopt), kinematic(false){}

Vector2F Entity::forward() const
{
    return {-std::sin(this->get_rotation()), std::cos(this->get_rotation())};
}

void Entity::update([[maybe_unused]] EntityManager& manager, float delta_time)
{
    if(!this->is_kinematic())
    {
        if(this->has_target())
            this->velocity = {(*this->get_target() - this->position_screenspace).normalised() * Entity::default_speed, 0.0f};
    }
    else
        this->velocity = {};
    GameSprite::update(delta_time);
}

bool Entity::is_moving() const
{
    return this->velocity.length() > 0.0f;
}

bool Entity::is_dead() const
{
    return this->health == 0;
}

unsigned int Entity::get_health() const
{
    return this->health;
}

void Entity::set_health(unsigned int health)
{
    this->health = health;
}

void Entity::add_health(unsigned int health)
{
    this->health += health;
}

void Entity::remove_health(unsigned int health)
{
    if (health > this->health)
        this->health = 0;
    else
        this->health -= health;
}

const Vector2F* Entity::get_target() const
{
    if(this->has_target())
        return &this->target.value();
    else
        return nullptr;
}

bool Entity::has_target() const
{
    return this->target.has_value();
}

void Entity::set_target(Vector2F target)
{
    this->target = target;
}

void Entity::remove_target()
{
    this->target = std::nullopt;
}

bool Entity::is_kinematic() const
{
    return this->kinematic;
}

void Entity::set_kinematic(bool kinematic)
{
    this->kinematic = kinematic;
}

Player* Entity::get_closest_player(EntityManager& manager)
{
    Player* closest = nullptr;
    auto players = manager.get_players();
    if(players.size() > 0)
        closest = players.front();
    for(Player* player : players)
    {
        float current_distance = (closest->position_screenspace - this->position_screenspace).length();
        float next_distance = (player->position_screenspace - this->position_screenspace).length();
        if(next_distance < current_distance)
            closest = player;
    }
    return closest;
}