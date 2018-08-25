#include "sprite_collection.hpp"

SpriteCollection::SpriteCollection(): AssetBuffer(), player(this->emplace_texture("player", load_player_texture())), large_asteroid(this->emplace_texture("large_asteroid", load_large_asteroid_texture())), medium_asteroid(this->emplace_texture("medium_asteroid", load_medium_asteroid_texture())), small_asteroid(this->emplace_texture("small_asteroid", load_small_asteroid_texture())), ufo(this->emplace_texture("ufo", load_ufo_texture())){}

const Texture& SpriteCollection::get_player() const
{
    return this->player;
}

const Texture& SpriteCollection::get_large_asteroid() const
{
    return this->large_asteroid;
}

const Texture& SpriteCollection::get_medium_asteroid() const
{
    return this->medium_asteroid;
}

const Texture& SpriteCollection::get_small_asteroid() const
{
    return this->small_asteroid;
}

const Texture& SpriteCollection::get_ufo() const
{
    return this->ufo;
}

Texture SpriteCollection::load_player_texture()
{
    return {"../res/textures/player.png"};
}

Texture SpriteCollection::load_large_asteroid_texture()
{
    return {"../res/textures/rock_large.png"};
}

Texture SpriteCollection::load_medium_asteroid_texture()
{
    return {"../res/textures/rock_medium.png"};
}

Texture SpriteCollection::load_small_asteroid_texture()
{
    return {"../res/textures/rock_small.png"};
}

Texture SpriteCollection::load_ufo_texture()
{
    return {"../res/textures/ufo.png"};
}
