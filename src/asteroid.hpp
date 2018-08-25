#ifndef TOPAZASTEROIDS_ASTEROID_HPP
#define TOPAZASTEROIDS_ASTEROID_HPP
#include "graphics/sprite.hpp"


class Asteroid : public Sprite
{
public:
    enum class Type{LARGE, MEDIUM, SMALL};
    Asteroid(Vector2I position, float rotation, Vector2F scale, Type type, Texture* asteroid_texture);
    Type get_type() const;
private:
    Type type;
    Texture* texture;
};


#endif //TOPAZASTEROIDS_ASTEROID_HPP
