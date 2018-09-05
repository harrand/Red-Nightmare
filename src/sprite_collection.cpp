#include "sprite_collection.hpp"

SpriteCollection::SpriteCollection(): AssetBuffer()
{
    this->emplace_texture("player", load_player_idle_texture());
    this->emplace_animated_texture("player_right", load_player_right_animation(), 1);
    this->emplace_animated_texture("player_left", load_player_left_animation(), 1);
}

Texture& SpriteCollection::get_player_idle()
{
    return *this->find_texture("player");
}

AnimatedTexture& SpriteCollection::get_player_right()
{
    return *this->find_animated_texture("player_right");
}

AnimatedTexture& SpriteCollection::get_player_left()
{
    return *this->find_animated_texture("player_left");
}

Texture SpriteCollection::load_player_idle_texture()
{
    return {"../res/textures/player/idle.png"};
}

PolyFrameTexture::FrameMap SpriteCollection::load_player_right_animation()
{
    return {{0, Texture{"../res/textures/player/special.png"}}, {1, Texture{Bitmap<PixelRGBA>{{{0, 255, 0, 255}, {255, 0, 255, 255}}, 2, 1}}}, {2, Texture{Bitmap<PixelRGBA>{{{255, 0, 0, 255}}, 1, 1}}}};
}

PolyFrameTexture::FrameMap SpriteCollection::load_player_left_animation()
{
    return {{0, {"../res/textures/player/left_1.png"}}, {1, {"../res/textures/player/left_2.png"}}};
}