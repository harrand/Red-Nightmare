//
// Created by Harrand on 25/08/2018.
//

#include "player.hpp"
#include "audio/audio.hpp"
#include "entity_manager.hpp"

Player::Player(Vector2I position, float rotation, Vector2F scale, const Texture *player_texture) : Sprite(position, rotation, scale, player_texture), PhysicsObject(1.0f), texture(player_texture){}

Vector2F Player::forward() const
{
    return {-std::sin(this->get_rotation()), std::cos(this->get_rotation())};
}

void Player::update(float delta)
{
    // This lambda performs verlet-integration. This is cheaper than forest-ruth but is slightly less accurate.
    auto verlet_integration = [&](float delta)
    {
        float half_delta = delta * 0.5f;
        auto vel = this->velocity * half_delta;
        this->position_screenspace.x += vel.x;
        this->position_screenspace.y += vel.y;
        this->set_rotation(this->get_rotation() + (this->angular_velocity * half_delta).z);
        PhysicsObject::update(delta);
        vel = this->velocity * half_delta;
        this->position_screenspace.x += vel.x;
        this->position_screenspace.y += vel.y;
        this->set_rotation (this->get_rotation() + (this->angular_velocity * half_delta).z);
    };
    // Perform forest-ruth motion integration. It utilises verlet-integration, so it more expensive but yields more accurate results.
    using namespace tz::utility::numeric;
    verlet_integration(delta * static_cast<float>(consts::forest_ruth_coefficient));
    verlet_integration(delta * static_cast<float>(consts::forest_ruth_complement));
    verlet_integration(delta * static_cast<float>(consts::forest_ruth_coefficient));
}

std::optional<AABB> Player::get_boundary() const
{
    Vector2F half_scale = this->scale / 2.0f;
    Vector3F centre = {static_cast<float>(this->position_screenspace.x), static_cast<float>(this->position_screenspace.y), 0.0f};
    Vector3F minimum = {centre.x - half_scale.x, centre.y - half_scale.y, 0.0f};
    Vector3F maximum = {centre.x + half_scale.x, centre.y + half_scale.y, 0.0f};
    return AABB{minimum, maximum};
}

Bullet& Player::shoot(EntityManager& entity_manager)
{
    Player::play_shoot_sound();
    return entity_manager.emplace<Bullet>(this->position_screenspace, this->forward(), &entity_manager.get_sprite_collection().get_bullet());
}

void Player::play_shoot_sound()
{
    tz::audio::play_async(AudioClip{"../res/sounds/shoot.wav"});
}