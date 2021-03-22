#ifndef REDNIGHTMARE_SPRITE_HPP
#define REDNIGHTMARE_SPRITE_HPP
#include "gl/texture.hpp"
#include <array>

namespace rn
{
    enum class SpriteState : std::size_t
    {
        Up,
        Down,
        Left,
        Right,
        Idle,
        Dead,
        Casting,

        Size
    };

    class Sprite
    {
    public:
        using TextureMap = std::array<tz::gl::BindlessTextureHandle, static_cast<std::size_t>(SpriteState::Size)>;

        Sprite(TextureMap sprite_textures, SpriteState default_state = SpriteState::Idle);
        
        SpriteState get_state() const;
        void set_state(SpriteState state);

        tz::gl::BindlessTextureHandle get_texture() const;
    private:
        SpriteState state;
        TextureMap textures;
    };
}

#endif // REDNIGHTMARE_SPRITE_HPP