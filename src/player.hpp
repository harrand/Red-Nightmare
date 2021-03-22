#ifndef TOPAZASTEROIDS_PLAYER_HPP
#define TOPAZASTEROIDS_PLAYER_HPP
#include "entity.hpp"

class EntityManager;

class Player : public Entity
{
public:
    Player(Vector2F position, float rotation, Vector2F scale, const Texture* player_texture);
    virtual void update(EntityManager& manager, float delta_time) override;
    virtual void on_death(EntityManager& manager) override;
    unsigned int get_souls() const;
    void set_souls(unsigned int souls);
    void add_souls(unsigned int souls);
    void remove_souls(unsigned int souls);
    static constexpr float default_speed = 0.15f;
private:
    unsigned int souls;
};


#endif //TOPAZASTEROIDS_PLAYER_HPP
