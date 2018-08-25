#include "asteroid.hpp"

Asteroid::Asteroid(Vector2I position, float rotation, Vector2F scale, Asteroid::Type type, Texture* asteroid_texture): Sprite(position, rotation, scale, asteroid_texture), type(type){}

Asteroid::Type Asteroid::get_type() const
{
    return this->type;
}
