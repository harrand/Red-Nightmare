#ifndef REDNIGHTMARE_SPRITE_MANAGER_HPP
#define REDNIGHTMARE_SPRITE_MANAGER_HPP
#include "gl/texture.hpp"
#include "sprite.hpp"
#include <map>

namespace rn
{
    struct SpriteType
    {
        const char* name;
        rn::SpriteState state;

        bool operator<(const SpriteType& rhs) const;
    };

    class SpriteTextureStorage
    {
    public:
        SpriteTextureStorage() = default;
        template<class PixelType>
        void add_texture(const char* base_sprite_name, rn::SpriteState state, const tz::gl::Image<PixelType>& image);
        rn::Sprite get(const char* base_sprite_name) const;
    private:
        std::map<SpriteType, tz::gl::Texture> tex_storage;
    };
}

#include "sprite_manager.inl"
#endif // REDNIGHTMARE_SPRITE_MANAGER_HPP