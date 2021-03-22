#ifndef TOPAZASTEROIDS_ENTITY_MANAGER_HPP
#define TOPAZASTEROIDS_ENTITY_MANAGER_HPP
#include "core/listener.hpp"
#include "core/scene.hpp"
#include "sprite_collection.hpp"
#include "player.hpp"
#include "enemies/nightmare.hpp"
#include "enemies/ghost.hpp"
#include "cursor.hpp"
#include "orbs/fireball.hpp"
#include "orbs/frostball.hpp"
#include "orbs/blackball.hpp"

class EntityManager : public Scene
{
public:
    EntityManager(Window& event_window);
    const Window& get_window() const;
    const Font& get_default_font() const;
    bool has_player() const;
    bool has_any_alive_player() const;
    Cursor& spawn_cursor();
    Fireball& spawn_fireball(Vector2F position, float rotation, Vector2F scale);
    Frostball& spawn_frostball(Vector2F position, float rotation, Vector2F scale);
    Blackball& spawn_blackball(Vector2F position, float rotation, Vector2F scale);
    Player& spawn_player(Vector2F position, float rotation, Vector2F scale);
    Ghost& spawn_ghost(Vector2F position, float rotation, Vector2F scale);
    Nightmare& spawn_nightmare(Vector2F position, float rotation, Vector2F scale);
    virtual void update(float delta) override;
    KeyListener& get_key_listener();
    const MouseListener& get_mouse_listener() const;
    bool screen_wrapping_enabled() const;
    std::optional<Vector2I> get_screen_wrapping_bounds() const;
    void enable_screen_wrapping(Vector2I bounds);
    void disable_screen_wrapping();
    SpriteCollection& get_sprite_collection();
    std::vector<Entity*> get_entities();
    std::vector<const Entity*> get_entities() const;
    std::vector<Player*> get_players();
    std::vector<const Player*> get_players() const;
    std::vector<Ghost*> get_ghosts();
    std::vector<Nightmare*> get_nightmares();

    friend class Player;
protected:
    void handle_screen_wrapping();
    Vector2F scale_normalised_position(Vector2F normalised_position);
    Window& window;
    Font default_font;
    KeyListener key_listener;
    MouseListener mouse_listener;
    SpriteCollection sprite_collection;
    Mesh quad;
    std::optional<Vector2I> screen_wrapping_bounds;
};


#endif //TOPAZASTEROIDS_ENTITY_MANAGER_HPP
