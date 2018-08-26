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
    return this->emplace<Player>(position, rotation, scale, &this->sprite_collection.get_player());
}

Asteroid& EntityManager::spawn_asteroid(Vector2F position, float rotation, Vector2F scale, Asteroid::Type type)
{
    const Texture* asteroid_texture = nullptr;
    switch(type)
    {
        case Asteroid::Type::LARGE:
            asteroid_texture = &this->sprite_collection.get_large_asteroid();
            break;
        case Asteroid::Type::MEDIUM:
            asteroid_texture = &this->sprite_collection.get_medium_asteroid();
            break;
        case Asteroid::Type::SMALL:
            asteroid_texture = &this->sprite_collection.get_small_asteroid();
            break;
    }
    return this->emplace<Asteroid>(position, rotation, scale, type, asteroid_texture);
}

void EntityManager::update(float delta)
{
    for(Player* player : this->get_players())
    {
        if(this->key_listener.is_key_pressed("A"))
            player->set_rotation(player->get_rotation() + delta * 10);
        if(this->key_listener.is_key_pressed("D"))
            player->set_rotation(player->get_rotation() - delta * 10);
        if(this->key_listener.is_key_pressed("Space"))
        {
            Vector2F forward = player->forward() * 30.0f;
            player->clear_forces();
            player->add_force({forward, 0.0f});
        }
        else
        {
            player->clear_forces();
            player->velocity *= 0.95f;
        }
        if(this->key_listener.catch_key_pressed("F"))
            player->shoot(*this);
        player->update(delta);
    }
    for(Bullet* bullet : this->get_bullets())
    {
        bullet->update(delta);
    }
    this->handle_screen_wrapping();
    Scene::update(delta);
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
        bool needs_erasing = false;
        Bullet* bullet_component = dynamic_cast<Bullet*>(&sprite);
        if(position.x < 0)
        {
            if(bullet_component != nullptr)
                needs_erasing = true;
            position.x = wrap_bounds.x;
        }
        else if(position.x > wrap_bounds.x)
        {
            if(bullet_component != nullptr)
                needs_erasing = true;
            position.x = 0;
        }
        if(position.y < 0)
        {
            if(bullet_component != nullptr)
                needs_erasing = true;
            position.y = wrap_bounds.y;
        }
        else if(position.y > wrap_bounds.y)
        {
            if(bullet_component != nullptr)
                needs_erasing = true;
            position.y = 0;
        }
        if(needs_erasing)
            this->remove_sprite(sprite);
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

std::vector<Bullet*> EntityManager::get_bullets()
{
    std::vector<Bullet*> bullets;
    for(auto& sprite_ptr : this->heap_sprites)
    {
        Bullet* bullet_component = dynamic_cast<Bullet*>(sprite_ptr.get());
        if(bullet_component != nullptr)
            bullets.push_back(bullet_component);
    }
    for(Sprite& sprite : this->stack_sprites)
    {
        Bullet* bullet_component = dynamic_cast<Bullet*>(&sprite);
        if(bullet_component != nullptr)
            bullets.push_back(bullet_component);
    }
    return bullets;
}