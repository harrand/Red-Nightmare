#include "asteroid.hpp"
#include "audio_manager.hpp"
#include "entity_manager.hpp"

Asteroid::Asteroid(Vector2F position, float rotation, Vector2F scale, Asteroid::Type type, const Texture* asteroid_texture): DynamicSprite(1.0f, position, rotation, scale, asteroid_texture), type(type){}

void Asteroid::on_collision(Player &player)
{
    player.remove_lives(1);
    Player::play_struck_sound();
}

void Asteroid::on_collision(Asteroid& asteroid)
{
    this->velocity *= -1.0f;
}

void Asteroid::explode(EntityManager& manager)
{
    switch(this->type)
    {
        case Type::LARGE:
            this->spawn_daughters(manager, Type::MEDIUM);
            break;
        case Type::MEDIUM:
            this->spawn_daughters(manager, Type::SMALL);
            break;
        case Type::SMALL:
            break;
    }
    manager.remove_sprite(*this);
}

Asteroid::Type Asteroid::get_type() const
{
    return this->type;
}

void Asteroid::play_explosion_sound()
{
    AudioManager::play_asteroid_explosion_sound();
}

int Asteroid::get_score_on_kill(Asteroid::Type type)
{
    switch(type)
    {
        case Asteroid::Type::LARGE:
            return Asteroid::score_large_asteroid;
        case Asteroid::Type::MEDIUM:
            return Asteroid::score_medium_asteroid;
        case Asteroid::Type::SMALL:
            return Asteroid::score_small_asteroid;
    }
    return 0;
}

std::pair<Asteroid*, Asteroid*> Asteroid::spawn_daughters(EntityManager& manager, Asteroid::Type daughter_type)
{
    Asteroid& daughter_a = manager.spawn_asteroid(this->position_screenspace, this->get_rotation(), this->scale / 2.0f, daughter_type);
    Asteroid& daughter_b = manager.spawn_asteroid(this->position_screenspace, this->get_rotation(), this->scale / 2.0f, daughter_type);
    daughter_a.velocity = this->velocity * -std::sqrt(2);
    daughter_b.velocity = this->velocity * std::sqrt(2);
    return {&daughter_a, &daughter_b};
}

void Asteroid::on_collision(PhysicsObject& other)
{
    Player* player_component = dynamic_cast<Player*>(&other);
    if(player_component != nullptr)
        this->on_collision(*player_component);
    Asteroid* asteroid_component = dynamic_cast<Asteroid*>(&other);
    if(asteroid_component != nullptr)
        this->on_collision(*asteroid_component);
}
