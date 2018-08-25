#include "core/window.hpp"
#include "core/topaz.hpp"
#include "utility/time.hpp"
#include "entity_manager.hpp"

void init();

int main()
{
    tz::initialise();
    init();
    tz::terminate();
    return 0;
}

void init()
{
    Window wnd("Asteroids", 0, 30, 800, 600);
    wnd.set_swap_interval_type(Window::SwapIntervalType::LATE_SWAP_TEARING);
    Shader gui_shader{"../src/shaders/gui"};
    Shader render_shader{"../src/shaders/3D"};
    EntityManager manager{wnd};
    manager.enable_screen_wrapping({wnd.get_width(), wnd.get_height()});
    manager.spawn_player({400, 300}, 0.0f, {40.0f, 40.0f});
    Camera camera;
    Timer tick_timer;
    constexpr int tps = 60;
    while(!wnd.is_close_requested())
    {
        tick_timer.update();
        using namespace tz::graphics;
        wnd.clear(BufferBit::COLOUR_AND_DEPTH);
        if(tick_timer.millis_passed(1000.0f / tps))
        {
            manager.update(tick_timer.get_range() / 1000.0f);
            tick_timer.reload();
        }
        manager.render(render_shader, &gui_shader, camera, {wnd.get_width(), wnd.get_height()});
        wnd.update(gui_shader, nullptr);
    }
}