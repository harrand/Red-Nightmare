#include "sprite_manager.hpp"

namespace rn
{
    bool SpriteType::operator==(const SpriteType& rhs) const
    {
        return std::string{this->name} == rhs.name && this->state == rhs.state;
    }

    std::size_t SpriteType::HashFunction::operator()(const SpriteType& type) const
    {
        std::size_t lhs_hash = std::hash<std::string>{}(type.name);
        lhs_hash ^= std::hash<std::size_t>{}(static_cast<std::size_t>(type.state));
        return lhs_hash;
    }

    void SpriteTextureStorage::update(float delta_millis)
    {
        for(auto&[type, tex] : this->tex_storage)
        {
            tex.update(delta_millis);
        }
    }

    rn::Sprite SpriteTextureStorage::get(const char* base_sprite_name) const
    {
        SpriteType type{base_sprite_name, rn::SpriteState::Up};
        auto map_contains_type = [this, type]()->bool
        {
            return this->tex_storage.find(type) != this->tex_storage.end();
        };
        auto assert_contains_type = [this, base_sprite_name, type, map_contains_type]()
        {
            const char* state_name;
            switch(type.state)
            {
                case SpriteState::Up:
                    state_name = "Up";
                break;
                case SpriteState::Down:
                    state_name = "Down";
                break;
                case SpriteState::Left:
                    state_name = "Left";
                break;
                case SpriteState::Right:
                    state_name = "Right";
                break;
                case SpriteState::Idle:
                    state_name = "Idle";
                break;
                case SpriteState::Dead:
                    state_name = "Dead";
                break;
                case SpriteState::Casting:
                    state_name = "Casting";
                break;
                default:
                    state_name = "Unknown";
                break;
            }
            topaz_assertf(map_contains_type(), "rn::SpriteTextureStorage::get(%s): No texture has been added for sprite state %s.", base_sprite_name, state_name);
        };
        // Now do each state.
        Sprite::TextureMap sprite_textures;
        for(std::size_t i = 0; i < static_cast<std::size_t>(SpriteState::Size); i++)
        {
            type.state = static_cast<SpriteState>(i);
            assert_contains_type();
            sprite_textures[i] = handleify(this->tex_storage.at(type));
        }

        return {sprite_textures};
    }

    void SpriteTextureStorage::enum_sprite_names(std::vector<const char*>& names) const
    {
        names.reserve(names.size() + this->sprite_names.size());
        for(const std::string& name : this->sprite_names)
        {
            names.push_back(name.c_str());
        }
    }
}