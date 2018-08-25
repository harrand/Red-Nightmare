#ifndef TOPAZASTEROIDS_SPRITE_COLLECTION_HPP
#define TOPAZASTEROIDS_SPRITE_COLLECTION_HPP
#include "graphics/asset.hpp"

class SpriteCollection : protected AssetBuffer
{
public:
    SpriteCollection();
    const Texture& get_player() const;
    const Texture& get_large_asteroid() const;
    const Texture& get_medium_asteroid() const;
    const Texture& get_small_asteroid() const;
    const Texture& get_ufo() const;
    const Texture& get_bullet() const;
    static Texture load_player_texture();
    static Texture load_large_asteroid_texture();
    static Texture load_medium_asteroid_texture();
    static Texture load_small_asteroid_texture();
    static Texture load_ufo_texture();
    static Texture load_bullet_texture();
private:
    Texture& player;
    Texture& large_asteroid;
    Texture& medium_asteroid;
    Texture& small_asteroid;
    Texture& ufo;
    Texture& bullet;
};


#endif //TOPAZASTEROIDS_SPRITE_COLLECTION_HPP
