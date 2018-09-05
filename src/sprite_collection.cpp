#include "sprite_collection.hpp"

SpriteCollection::SpriteCollection(): AssetBuffer(), player_idle(this->emplace_texture("player", load_player_idle_texture())){}

const Texture& SpriteCollection::get_player_idle() const
{
    return this->player_idle;
}

Texture SpriteCollection::load_player_idle_texture()
{
    return {"../res/textures/player/idle.png"};
}