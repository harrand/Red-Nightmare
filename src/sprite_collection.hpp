#ifndef TOPAZASTEROIDS_SPRITE_COLLECTION_HPP
#define TOPAZASTEROIDS_SPRITE_COLLECTION_HPP
#include "graphics/asset.hpp"

class SpriteCollection : protected AssetBuffer
{
public:
    SpriteCollection();
    const Texture& get_player_idle() const;
    static Texture load_player_idle_texture();
private:
    Texture& player_idle;
};


#endif //TOPAZASTEROIDS_SPRITE_COLLECTION_HPP
