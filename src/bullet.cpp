//
// Created by Harrand on 25/08/2018.
//

#include "bullet.hpp"
#include "entity_manager.hpp"

Bullet::Bullet(Vector2F position, Vector2F velocity, const Texture* bullet_texture, EntityManager& manager, DynamicSprite* shooter): DynamicSprite(1.0f, position, 0.0f, {Bullet::bullet_size, Bullet::bullet_size}, bullet_texture, velocity * Bullet::base_bullet_speed_multiplier), manager(manager), shooter(shooter)
{
    if(this->shooter != nullptr)
        this->velocity += this->shooter->velocity;
}

void Bullet::on_collision(Asteroid& asteroid)
{
    Player* player_component = dynamic_cast<Player*>(this->shooter);
    if(player_component != nullptr)
    {
        player_component->set_score(player_component->get_score() + Asteroid::get_score_on_kill(asteroid.get_type()));
    }
    this->manager.remove_sprite(*this);
    asteroid.explode(this->manager);
    Asteroid::play_explosion_sound();
}

void Bullet::on_collision(Player& player)
{
    // A player's own bullet cannot collide with itself.
    if(&player == this->shooter)
        return;
    std::cout << "bullet collided with player.\n";
}

void Bullet::on_collision(PhysicsObject& other)
{
    Asteroid* asteroid_component = dynamic_cast<Asteroid*>(&other);
    Player* player_component = dynamic_cast<Player*>(&other);
    if(asteroid_component != nullptr)
        this->on_collision(*asteroid_component);
    if(player_component != nullptr)
        this->on_collision(*player_component);
}