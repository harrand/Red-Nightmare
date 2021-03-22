#include "sprite_manager.hpp"

namespace rn
{
    bool SpriteType::operator<(const SpriteType& rhs) const
    {
        bool name_later = std::string{this->name} < std::string{rhs.name};
        bool state_later = static_cast<std::size_t>(this->state) < static_cast<std::size_t>(rhs.state);
        return name_later && state_later;
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
            sprite_textures[i] = this->tex_storage.at(type).get_terminal_handle();
        }

        return {sprite_textures};
    }
}