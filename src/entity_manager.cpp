#include "entity_manager.hpp"

EntityManager::EntityManager(Window& wnd): Scene(), key_listener(wnd), mouse_listener(wnd), sprite_collection(), quad(tz::util::gui::gui_quad()), screen_wrapping_bounds(std::nullopt){}

bool EntityManager::has_player() const
{
    bool contains = false;
    for(const auto& sprite_ptr : this->heap_sprites)
        if(dynamic_cast<Player*>(sprite_ptr.get()) != nullptr)
            contains = true;
    return contains;
}

Player& EntityManager::spawn_player(Vector2F position, float rotation, Vector2F scale)
{
    return this->emplace<Player>(position, rotation, scale, &this->sprite_collection.get_player_idle());
}

void EntityManager::update(float delta)
{
    Scene::update(delta);
    for(Player* player : this->get_players())
    {
        player->velocity = {};
        if(this->key_listener.is_key_pressed("W"))
            player->velocity += {0.0f, Player::default_speed, 0.0f};
        if(this->key_listener.is_key_pressed("S"))
            player->velocity += {0.0f, -Player::default_speed, 0.0f};
        if(this->key_listener.is_key_pressed("A"))
            player->velocity += {-Player::default_speed, 0.0f, 0.0f};
        if(this->key_listener.is_key_pressed("D"))
            player->velocity += {Player::default_speed, 0.0f, 0.0f};
        if(player->velocity.x > 0.0f)
            player->set_animation(&this->sprite_collection.get_player_right());
        else if(player->velocity.x < 0.0f)
            player->set_animation(&this->sprite_collection.get_player_left());
        else
            player->set_texture(&this->sprite_collection.get_player_idle());
        player->update(delta);
    }
    this->handle_screen_wrapping();
}

bool EntityManager::screen_wrapping_enabled() const
{
    return this->screen_wrapping_bounds.has_value();
}

std::optional<Vector2I> EntityManager::get_screen_wapping_bounds() const
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

const SpriteCollection& EntityManager::get_sprite_collection() const
{
    return this->sprite_collection;
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

std::vector<Player*> EntityManager::get_players()
{
    std::vector<Player*> players;
    for(auto& sprite_ptr : this->heap_sprites)
    {
        Player* player_component = dynamic_cast<Player*>(sprite_ptr.get());
        if(player_component != nullptr)
            players.push_back(player_component);
    }
    for(Sprite& sprite : this->stack_sprites)
    {
        Player* player_component = dynamic_cast<Player*>(&sprite);
        if(player_component != nullptr)
            players.push_back(player_component);
    }
    return players;
}