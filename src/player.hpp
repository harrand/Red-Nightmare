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
    static void play_shoot_sound();
private:
    const Texture* texture;
    int score;
};


#endif //TOPAZASTEROIDS_PLAYER_HPP
