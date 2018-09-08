#ifndef TOPAZASTEROIDS_ENTITY_MANAGER_HPP
#define TOPAZASTEROIDS_ENTITY_MANAGER_HPP
#include "core/listener.hpp"
#include "core/scene.hpp"
#include "sprite_collection.hpp"
#include "player.hpp"
#include "ghost.hpp"
#include "cursor.hpp"
#include "fireball.hpp"

class EntityManager : public Scene
{
public:
    EntityManager(Window& event_window);
    bool has_player() const;
    Cursor& spawn_cursor();
    Fireball& spawn_fireball(Vector2F position, float rotation, Vector2F scale);
    Player& spawn_player(Vector2F position, float rotation, Vector2F scale);
    Ghost& spawn_ghost(Vector2F position, float rotation, Vector2F scale);
    virtual void update(float delta) override;
    KeyListener& get_key_listener();
    const MouseListener& get_mouse_listener() const;
    bool screen_wrapping_enabled() const;
    std::optional<Vector2I> get_screen_wrapping_bounds() const;
    void enable_screen_wrapping(Vector2I bounds);
    void disable_screen_wrapping();
    SpriteCollection& get_sprite_collection();

    friend class Entity;
    friend class Cursor;
    friend class Fireball;
protected:
    void handle_screen_wrapping();
    std::vector<Entity*> get_entities();
    std::vector<Player*> get_players();
    std::vector<Ghost*> get_ghosts();
    KeyListener key_listener;
    MouseListener mouse_listener;
    SpriteCollection sprite_collection;
    Mesh quad;
    std::optional<Vector2I> screen_wrapping_bounds;
};


#endif //TOPAZASTEROIDS_ENTITY_MANAGER_HPP
