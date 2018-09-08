#ifndef TOPAZASTEROIDS_SPRITE_COLLECTION_HPP
#define TOPAZASTEROIDS_SPRITE_COLLECTION_HPP
#include "graphics/asset.hpp"

class SpriteCollection : protected AssetBuffer
{
public:
    SpriteCollection();
    Texture& get_player_idle();
    Texture& get_player_dead();
    AnimatedTexture& get_player_right();
    AnimatedTexture& get_player_left();
    Texture& get_ghost_idle();
    Texture& get_ghost_dead();
    AnimatedTexture& get_ghost_right();
    AnimatedTexture& get_ghost_left();
    Texture& get_player_special();
    Texture& get_on_rune();
    Texture& get_off_rune();
    Texture& get_fireball();
    static Texture load_player_idle_texture();
    static Texture load_player_dead_texture();
    static PolyFrameTexture::FrameMap load_player_right_animation();
    static PolyFrameTexture::FrameMap load_player_left_animation();
    static Texture load_player_special_texture();
    static Texture load_on_rune_texture();
    static Texture load_off_rune_texture();
    static Texture load_fireball_texture();
    static Texture load_ghost_idle_texture();
    static Texture load_ghost_dead_texture();
    static PolyFrameTexture::FrameMap load_ghost_right_animation();
    static PolyFrameTexture::FrameMap load_ghost_left_animation();
};


#endif //TOPAZASTEROIDS_SPRITE_COLLECTION_HPP
