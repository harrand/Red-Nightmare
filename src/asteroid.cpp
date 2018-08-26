#include "asteroid.hpp"

Asteroid::Asteroid(Vector2F position, float rotation, Vector2F scale, Asteroid::Type type, const Texture* asteroid_texture): Sprite(position, rotation, scale, asteroid_texture), type(type){}

Asteroid::Type Asteroid::get_type() const
{
    return this->type;
}
