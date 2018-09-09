#include "entity_manager.hpp"

EntityManager::EntityManager(Window& wnd): Scene(), window(wnd), default_font("../res/fonts/comfortaa.ttf", 24), key_listener(wnd), mouse_listener(wnd), sprite_collection(), quad(tz::util::gui::gui_quad()), screen_wrapping_bounds(std::nullopt){}

const Window& EntityManager::get_window() const
{
    return this->window;
}

const Font& EntityManager::get_default_font() const
{
    return this->default_font;
}

bool EntityManager::has_player() const
{
    return !this->get_players().empty();
}

bool EntityManager::has_any_alive_player() const
{
    for(const Player* player : this->get_players())
        if(!player->is_dead())
            return true;
    return false;
}

Cursor& EntityManager::spawn_cursor()
{
    return this->emplace<Cursor>(&this->sprite_collection.get_off_rune());
}

Fireball& EntityManager::spawn_fireball(Vector2F position, float rotation, Vector2F scale)
{
    return this->emplace<Fireball>(position, rotation, scale, &this->sprite_collection.get_fireball());
}

Frostball& EntityManager::spawn_frostball(Vector2F position, float rotation, Vector2F scale)
{
    return this->emplace<Frostball>(position, rotation, scale, &this->sprite_collection.get_frostball());
}

Blackball& EntityManager::spawn_blackball(Vector2F position, float rotation, Vector2F scale)
{
    return this->emplace<Blackball>(position, rotation, scale, &this->sprite_collection.get_blackball());
}

Player& EntityManager::spawn_player(Vector2F position, float rotation, Vector2F scale)
{
    return this->emplace<Player>(position, rotation, scale, &this->sprite_collection.get_player_idle());
}

Ghost& EntityManager::spawn_ghost(Vector2F position, float rotation, Vector2F scale)
{
    return this->emplace<Ghost>(position, rotation, scale, &this->sprite_collection.get_ghost_idle());
}

void EntityManager::update(float delta)
{
    Scene::update(delta);
    for(Entity* entity: this->get_entities())
        entity->update(*this, delta);
    this->handle_screen_wrapping();
}

KeyListener& EntityManager::get_key_listener()
{
    return this->key_listener;
}

const MouseListener& EntityManager::get_mouse_listener() const
{
    return this->mouse_listener;
}

bool EntityManager::screen_wrapping_enabled() const
{
    return this->screen_wrapping_bounds.has_value();
}

std::optional<Vector2I> EntityManager::get_screen_wrapping_bounds() const
{
    return this->screen_wrapping_bounds;
}

void EntityManager::enable_screen_wrapping(Vector2I bounds)
{
    this->screen_wrapping_bounds = bounds;
}

void EntityManager::disable_screen_wrapping()
{
    this->screen_wrapping_bounds = std::nullopt;
}

SpriteCollection& EntityManager::get_sprite_collection()
{
    return this->sprite_collection;
}

std::vector<Entity*> EntityManager::get_entities()
{
    std::vector<Entity*> entities;
    for(auto& sprite_ptr : this->heap_sprites)
    {
        Entity* entity_component = dynamic_cast<Entity*>(sprite_ptr.get());
        if(entity_component != nullptr)
            entities.push_back(entity_component);
    }
    for(Sprite& sprite : this->stack_sprites)
    {
        Entity* entity_component = dynamic_cast<Entity*>(&sprite);
        if(entity_component != nullptr)
            entities.push_back(entity_component);
    }
    return entities;
}

std::vector<const Entity*> EntityManager::get_entities() const
{
    std::vector<const Entity*> entities;
    for(auto& sprite_ptr : this->heap_sprites)
    {
        const Entity* entity_component = dynamic_cast<const Entity*>(sprite_ptr.get());
        if(entity_component != nullptr)
            entities.push_back(entity_component);
    }
    for(const Sprite& sprite : this->stack_sprites)
    {
        const Entity* entity_component = dynamic_cast<const Entity*>(&sprite);
        if(entity_component != nullptr)
            entities.push_back(entity_component);
    }
    return entities;
}

std::vector<Player*> EntityManager::get_players()
{
    std::vector<Player*> players;
    for(Entity* entity : this->get_entities())
    {
        Player* player_component = dynamic_cast<Player*>(entity);
        if(player_component != nullptr)
            players.push_back(player_component);
    }
    return players;
}

std::vector<const Player*> EntityManager::get_players() const
{
    std::vector<const Player*> players;
    for(const Entity* entity : this->get_entities())
    {
        const Player* player_component = dynamic_cast<const Player*>(entity);
        if(player_component != nullptr)
            players.push_back(player_component);
    }
    return players;
}

std::vector<Ghost*> EntityManager::get_ghosts()
{
    std::vector<Ghost*> ghosts;
    for(Entity* entity : this->get_entities())
    {
        Ghost* ghost_component = dynamic_cast<Ghost*>(entity);
        if(ghost_component != nullptr)
            ghosts.push_back(ghost_component);
    }
    return ghosts;
}

void EntityManager::handle_screen_wrapping()
{
    if(!this->screen_wrapping_enabled())
        return;
    auto wrap_bounds = this->screen_wrapping_bounds.value();
    for(Sprite& sprite : this->get_mutable_sprites())
    {
        auto& position = sprite.position_screenspace;
        if(position.x < 0)
        {
            position.x = wrap_bounds.x;
        }
        else if(position.x > wrap_bounds.x)
        {
            position.x = 0;
        }
        if(position.y < 0)
        {
            position.y = wrap_bounds.y;
        }
        else if(position.y > wrap_bounds.y)
        {
            position.y = 0;
        }
    }
}