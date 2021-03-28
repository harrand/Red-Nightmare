#ifndef REDNIGHTMARE_SPRITE_MANAGER_HPP
#define REDNIGHTMARE_SPRITE_MANAGER_HPP
#include "gl/texture.hpp"
#include "sprite.hpp"
#include "animated_texture.hpp"
#include <unordered_map>
#include <unordered_set>

namespace rn
{
    struct SpriteType
    {
        const char* name;
        rn::SpriteState state;

        bool operator==(const SpriteType& rhs) const;

        struct HashFunction
        {
            std::size_t operator()(const SpriteType& type) const;
        };
    };

    class SpriteTextureStorage
    {
    public:
        SpriteTextureStorage() = default;
        void update(float delta_millis);
        template<class PixelType>
        void add_texture(const char* base_sprite_name, rn::SpriteState state, const AnimatedTexture<tz::gl::Image<PixelType>>& image);
        rn::Sprite get(const char* base_sprite_name) const;
        void enum_sprite_names(std::vector<const char*>& names) const;
    private:
        std::unordered_map<SpriteType, AnimatedTexture<tz::gl::Texture>, SpriteType::HashFunction> tex_storage;
        std::unordered_set<std::string> sprite_names;
    };
}

#include "sprite_manager.inl"
#endif // REDNIGHTMARE_SPRITE_MANAGER_HPP