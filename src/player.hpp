#ifndef TOPAZASTEROIDS_PLAYER_HPP
#define TOPAZASTEROIDS_PLAYER_HPP
#include "entity.hpp"

class EntityManager;

class Player : public Entity
{
public:
    Player(Vector2F position, float rotation, Vector2F scale, const Texture* player_texture);
    virtual void update(EntityManager& manager, float delta_time) override;
    static void play_shoot_sound();
    static void play_struck_sound();
    static constexpr unsigned int default_health = 100;
    static constexpr float default_speed = 50.0f;
private:
    unsigned int health;
};


#endif //TOPAZASTEROIDS_PLAYER_HPP
