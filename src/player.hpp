#ifndef TOPAZASTEROIDS_PLAYER_HPP
#define TOPAZASTEROIDS_PLAYER_HPP
#include "game_sprite.hpp"

class EntityManager;

class Player : public GameSprite
{
public:
    Player(Vector2F position, float rotation, Vector2F scale, const Texture* player_texture);
    Vector2F forward() const;
    virtual void update(float delta_time) override;
    virtual std::optional<AABB> get_boundary() const;
    virtual void on_collision([[maybe_unused]] PhysicsObject& other){}
    bool is_dead() const;
    unsigned int get_health() const;
    void set_health(unsigned int health);
    void add_health(unsigned int health);
    void remove_health(unsigned int health);
    static void play_shoot_sound();
    static void play_struck_sound();
    static constexpr unsigned int default_health = 100;
    static constexpr float default_speed = 50.0f;
private:
    unsigned int health;
};


#endif //TOPAZASTEROIDS_PLAYER_HPP
