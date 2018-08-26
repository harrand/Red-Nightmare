#ifndef TOPAZASTEROIDS_ASTEROID_HPP
#define TOPAZASTEROIDS_ASTEROID_HPP
#include "physics/dynamic_sprite.hpp"

class EntityManager;

class Asteroid : public DynamicSprite
{
public:
    enum class Type{LARGE, MEDIUM, SMALL};
    Asteroid(Vector2F position, float rotation, Vector2F scale, Type type, const Texture* asteroid_texture);
    void explode(EntityManager& manager);
    Type get_type() const;
    static void play_explosion_sound();
    static int get_score_on_kill(Asteroid::Type type);
    static constexpr int score_large_asteroid = 20;
    static constexpr int score_medium_asteroid = 50;
    static constexpr int score_small_asteroid = 100;
private:
    std::pair<Asteroid*, Asteroid*> spawn_daughters(EntityManager& manager, Asteroid::Type daughter_type);
    Type type;
    const Texture* texture;
};


#endif //TOPAZASTEROIDS_ASTEROID_HPP
