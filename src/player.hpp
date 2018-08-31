#ifndef TOPAZASTEROIDS_PLAYER_HPP
#define TOPAZASTEROIDS_PLAYER_HPP
#include "physics/dynamic_sprite.hpp"
#include "bullet.hpp"

class EntityManager;

class Player : public DynamicSprite
{
public:
    Player(Vector2F position, float rotation, Vector2F scale, const Texture* player_texture);
    Vector2F forward() const;
    virtual void update(float delta) override;
    virtual std::optional<AABB> get_boundary() const;
    virtual void on_collision([[maybe_unused]] PhysicsObject& other){}
    Bullet& shoot(EntityManager& entity_manager);
    int get_score() const;
    void set_score(int score);
    bool is_dead() const;
    unsigned int get_lives() const;
    void set_lives(unsigned int lives);
    void add_lives(unsigned int lives);
    void remove_lives(unsigned int lives);
    static void play_shoot_sound();
    static void play_struck_sound();
    static constexpr unsigned int default_lives = 5;
private:
    const Texture* texture;
    int score;
    unsigned int lives;
};


#endif //TOPAZASTEROIDS_PLAYER_HPP
