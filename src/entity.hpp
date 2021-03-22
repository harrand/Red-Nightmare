//
// Created by Harrand on 07/09/2018.
//

#ifndef REDNIGHTMARE_ENTITY_HPP
#define REDNIGHTMARE_ENTITY_HPP
#include "game_sprite.hpp"

class Player;

class EntityManager;

class Entity : public GameSprite
{
public:
    Entity(Vector2F position, float rotation, Vector2F scale, const Texture* texture, unsigned int health = Entity::default_health, float speed = Entity::default_speed);
    virtual void update(EntityManager& manager, float delta_time);
    virtual void on_death([[maybe_unused]] EntityManager& manager){}
    Vector2F forward() const;
    bool is_moving() const;
    bool is_dead() const;
    unsigned int get_health() const;
    virtual void set_health(EntityManager& manager, unsigned int health);
    virtual void add_health(EntityManager& manager, unsigned int health);
    virtual void remove_health(EntityManager& manager, unsigned int health);
    float get_speed() const;
    void set_speed(float speed);
    const Vector2F* get_target() const;
    bool has_target() const;
    void set_target(Vector2F target);
    void remove_target();
    bool is_kinematic() const;
    void set_kinematic(bool kinematic);
    static constexpr unsigned int default_health = 100;
    static constexpr float default_speed = 0.1f;
protected:
    Player* get_closest_player(EntityManager& manager);
    unsigned int health;
    float speed;
    std::optional<Vector2F> target;
    bool kinematic;
};


#endif //REDNIGHTMARE_ENTITY_HPP
