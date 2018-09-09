#include "sprite_collection.hpp"

SpriteCollection::SpriteCollection(): AssetBuffer()
{
    this->emplace_texture("player", load_player_idle_texture());
    this->emplace_texture("player_dead", load_player_dead_texture());
    this->emplace_animated_texture("player_right", load_player_right_animation(), 10);
    this->emplace_animated_texture("player_left", load_player_left_animation(), 10);
    this->emplace_texture("player_special", load_player_special_texture());
    this->emplace_texture("on_rune", load_on_rune_texture());
    this->emplace_texture("off_rune", load_off_rune_texture());
    this->emplace_texture("fireball", load_fireball_texture());
    this->emplace_texture("frostball", load_frostball_texture());
    this->emplace_texture("blackball", load_blackball_texture());
    this->emplace_texture("ghost", load_ghost_idle_texture());
    this->emplace_texture("ghost_dead", load_ghost_dead_texture());
    this->emplace_animated_texture("ghost_right", load_ghost_right_animation(), 10);
    this->emplace_animated_texture("ghost_left", load_ghost_left_animation(), 10);
    this->emplace_texture("powerup_missile_growth", load_powerup_missile_growth_texture());
}

Texture& SpriteCollection::get_player_idle()
{
    return *this->find_texture("player");
}

Texture& SpriteCollection::get_player_dead()
{
    return *this->find_texture("player_dead");
}

AnimatedTexture& SpriteCollection::get_player_right()
{
    return *this->find_animated_texture("player_right");
}

AnimatedTexture& SpriteCollection::get_player_left()
{
    return *this->find_animated_texture("player_left");
}

Texture& SpriteCollection::get_ghost_idle()
{
    return *this->find_texture("ghost");
}

Texture& SpriteCollection::get_ghost_dead()
{
    return *this->find_texture("ghost_dead");
}

AnimatedTexture& SpriteCollection::get_ghost_right()
{
    return *this->find_animated_texture("ghost_right");
}

AnimatedTexture& SpriteCollection::get_ghost_left()
{
    return *this->find_animated_texture("ghost_left");
}

Texture& SpriteCollection::get_player_special()
{
    return *this->find_texture("player_special");
}

Texture& SpriteCollection::get_on_rune()
{
    return *this->find_texture("on_rune");
}

Texture& SpriteCollection::get_off_rune()
{
    return *this->find_texture("off_rune");
}

Texture& SpriteCollection::get_fireball()
{
    return *this->find_texture("fireball");
}

Texture& SpriteCollection::get_frostball()
{
    return *this->find_texture("frostball");
}

Texture& SpriteCollection::get_blackball()
{
    return *this->find_texture("blackball");
}

Texture& SpriteCollection::get_powerup_missile_growth()
{
    return *this->find_texture("powerup_missile_growth");
}

Texture SpriteCollection::load_player_idle_texture()
{
    return {"../res/textures/player/idle.png"};
}

Texture SpriteCollection::load_player_dead_texture()
{
    return {"../res/textures/player/dead.png"};
}

PolyFrameTexture::FrameMap SpriteCollection::load_player_right_animation()
{
    return {{0, {"../res/textures/player/right_1.png"}}, {1, {"../res/textures/player/right_2.png"}}};
}

PolyFrameTexture::FrameMap SpriteCollection::load_player_left_animation()
{
    return {{0, {"../res/textures/player/left_1.png"}}, {1, {"../res/textures/player/left_2.png"}}};
}

Texture SpriteCollection::load_player_special_texture()
{
    return {"../res/textures/player/special.png"};
}

Texture SpriteCollection::load_on_rune_texture()
{
    return {"../res/textures/rune_on.png"};
}

Texture SpriteCollection::load_off_rune_texture()
{
    return {"../res/textures/rune_off.png"};
}

Texture SpriteCollection::load_fireball_texture()
{
    return {"../res/textures/missile_fire.png"};
}

Texture SpriteCollection::load_frostball_texture()
{
    return {"../res/textures/missile_frost.png"};
}

Texture SpriteCollection::load_blackball_texture()
{
    return {"../res/textures/missile_black.png"};
}

Texture SpriteCollection::load_ghost_idle_texture()
{
    return {"../res/textures/ghost/idle.png"};
}

Texture SpriteCollection::load_ghost_dead_texture()
{
    return {"../res/textures/ghost/dead.png"};
}

PolyFrameTexture::FrameMap SpriteCollection::load_ghost_right_animation()
{
    return {{0, {"../res/textures/ghost/right_1.png"}}, {1, {"../res/textures/ghost/right_2.png"}}};
}

PolyFrameTexture::FrameMap SpriteCollection::load_ghost_left_animation()
{
    return {{0, {"../res/textures/ghost/left_1.png"}}, {1, {"../res/textures/ghost/left_2.png"}}};
}

Texture SpriteCollection::load_powerup_missile_growth_texture()
{
    return {"../res/textures/powerups/missile_growth.png"};
}