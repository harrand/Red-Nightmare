#include "core/tz.hpp"
#include "gl/frame.hpp"
#include "sprite_manager.hpp"
#include "gl/tz_stb_image/image_reader.hpp"
#include <cstdio>

rn::SpriteTextureStorage collate_sprites()
{
    rn::SpriteTextureStorage sts;
    using namespace tz::ext::stb;
    sts.add_texture("player", rn::SpriteState::Idle, read_image<tz::gl::PixelRGBA8>("res/textures/player/idle.png"));
    return sts;
}

int main()
{
    tz::initialise("Red Nightmare");
    {
        tz::IWindow& wnd = tz::get().window();
        wnd.get_frame()->set_clear_color(0.4f, 0.4f, 1.0f);

        rn::SpriteTextureStorage sprites = collate_sprites();

        while(!wnd.is_close_requested())
        {
            tz::update();
            wnd.update();
        }
    }
    tz::terminate();
}