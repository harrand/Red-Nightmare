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

    constexpr const char* get_sprite_state_name(SpriteState state)
    {
        switch(state)
        {
            case SpriteState::Up:
                return "Up";
            break;
            case SpriteState::Down:
                return "Down";
            break;
            case SpriteState::Left:
                return "Left";
            break;
            case SpriteState::Right:
                return "Right";
            break;
            case SpriteState::Idle:
                return "Idle";
            break;
            case SpriteState::Dead:
                return "Dead";
            break;
            case SpriteState::Casting:
                return "Casting";
            break;
            default:
                return "<Unknown Sprite State>";
            break;
        }
    }

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