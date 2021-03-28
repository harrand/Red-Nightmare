#include "sprite.hpp"
#include "core/debug/assert.hpp"

namespace rn
{
    Sprite::Sprite(Sprite::TextureMap sprite_textures, SpriteState default_state): state(default_state), textures(sprite_textures){}

    SpriteState Sprite::get_state() const
    {
        return this->state;
    }

    void Sprite::set_state(SpriteState state)
    {
        topaz_assert(state != SpriteState::Size, "rn::Sprite::set_sprite(SpriteState::Size): Illegal argument, must not be Size.");
        this->state = state;
    }

    tz::gl::BindlessTextureHandle Sprite::get_texture() const
    {
        return this->textures[static_cast<std::size_t>(this->state)].get_texture();
    }


}