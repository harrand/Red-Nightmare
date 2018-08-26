#ifndef TOPAZASTEROIDS_ENTITY_MANAGER_HPP
#define TOPAZASTEROIDS_ENTITY_MANAGER_HPP
#include "core/listener.hpp"
#include "core/scene.hpp"
#include "sprite_collection.hpp"
#include "player.hpp"
#include "asteroid.hpp"
#include "bullet.hpp"

class EntityManager : public Scene
{
public:
    EntityManager(Window& event_window);
    bool has_player() const;
    Player& spawn_player(Vector2F position, float rotation, Vector2F scale);
    Asteroid& spawn_asteroid(Vector2F position, float rotation, Vector2F scale, Asteroid::Type type);
    virtual void update(float delta) override;
    bool screen_wrapping_enabled() const;
    std::optional<Vector2I> get_screen_wapping_bounds() const;
    void enable_screen_wrapping(Vector2I bounds);
    void disable_screen_wrapping();
    const SpriteCollection& get_sprite_collection() const;
protected:
    void handle_screen_wrapping();
    std::vector<Player*> get_players();
    std::vector<Bullet*> get_bullets();
    KeyListener key_listener;
    MouseListener mouse_listener;
    SpriteCollection sprite_collection;
    Mesh quad;
    std::optional<Vector2I> screen_wrapping_bounds;
};


#endif //TOPAZASTEROIDS_ENTITY_MANAGER_HPP
