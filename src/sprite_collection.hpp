#ifndef TOPAZASTEROIDS_SPRITE_COLLECTION_HPP
#define TOPAZASTEROIDS_SPRITE_COLLECTION_HPP
#include "graphics/asset.hpp"

class SpriteCollection : protected AssetBuffer
{
public:
    SpriteCollection();
    Texture& get_player_idle();
    AnimatedTexture& get_player_right();
    AnimatedTexture& get_player_left();
    static Texture load_player_idle_texture();
    static PolyFrameTexture::FrameMap load_player_right_animation();
    static PolyFrameTexture::FrameMap load_player_left_animation();
};


#endif //TOPAZASTEROIDS_SPRITE_COLLECTION_HPP
