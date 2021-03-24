#ifndef REDNIGHTMARE_SPRITE_MANAGER_WINDOW_HPP
#define REDNIGHTMARE_SPRITE_MANAGER_WINDOW_HPP
#include "dui/window.hpp"
#include "sprite_manager.hpp"

namespace rn::dui
{
    class SpriteManagerWindow : public tz::dui::DebugWindow
    {
    public:
        SpriteManagerWindow(const rn::SpriteTextureStorage& storage);
        virtual void render() override;
    private:
        const rn::SpriteTextureStorage& storage;
    };
}

#endif // REDNIGHTMARE_SPRITE_MANAGER_WINDOW_HPP