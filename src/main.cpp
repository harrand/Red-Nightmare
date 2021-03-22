#include "core/window.hpp"
#include "core/topaz.hpp"
#include "utility/time.hpp"
#include "entity_manager.hpp"
#include "graphics/gui/display.hpp"

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
    Window wnd("Red Nightmare", 0, 30, 800, 600);
    wnd.set_swap_interval_type(Window::SwapIntervalType::LATE_SWAP_TEARING);
    Shader gui_shader{"../src/shaders/gui"};
    Shader render_shader{"../src/shaders/3D"};
    EntityManager manager{wnd};
    Label& hp_label = wnd.emplace_child<Label>(Vector2I{0, 0}, manager.get_default_font(), Vector3F{1.0f, 0.0f, 0.0f}, "HP: ");
    ProgressBar& nightmare_progress = wnd.emplace_child<ProgressBar>(Vector2I{0, hp_label.get_height() + 10}, Vector2I{100, 20});
    nightmare_progress.set_local_dimensions_normalised_space({0.2f, 0.1f});
    manager.enable_screen_wrapping({wnd.get_width(), wnd.get_height()});
    manager.spawn_cursor();
    //manager.spawn_frostball({wnd.get_width() / 2.0f + 40, wnd.get_height() / 2.0f}, 0.0f, {40, 40});
    //manager.spawn_fireball({wnd.get_width() / 2.0f, wnd.get_height() / 2.0f}, 0.0f, {40, 40});
    Entity* orb = &manager.spawn_fireball({wnd.get_width() / 2.0f + 20, wnd.get_height() / 2.0f - 40}, 0.0f, {40, 40});
    Player& player = manager.spawn_player({10, 10}, 0.0f, {100, 100});
    manager.spawn_ghost({200, 10}, 0.0f, {100, 100});
    Camera camera;
    Timer tick_timer;
    constexpr int tps = 200;
    LocalRandom rand;
    while(!wnd.is_close_requested())
    {
        static int count = 0;
        hp_label.set_text("HP: " + tz::utility::generic::cast::to_string(player.get_health()));
        tick_timer.update();
        using namespace tz::graphics;
        wnd.clear(BufferBit::COLOUR_AND_DEPTH);
        if(tick_timer.millis_passed(1000.0f / tps))
        {
            nightmare_progress.set_progress(player.get_souls() / 100.0f);
            if(player.get_souls() >= 10)
            {
                switch(++count)
                {
                    case 1:
                        manager.remove_sprite(*orb);
                        orb = &manager.spawn_frostball({wnd.get_width() / 2.0f, wnd.get_height() / 2.0f}, 0.0f, {40, 40});
                        player.remove_souls(100);
                        break;
                    default:
                        if(player.get_souls() >= 100 || count == 2)
                        {
                            manager.remove_sprite(*orb);
                            orb = &manager.spawn_blackball({wnd.get_width() / 2.0f, wnd.get_height() / 2.0f}, 0.0f, {40, 40});
                            player.remove_souls(100);
                            manager.spawn_nightmare({200, 10}, 0.0f, {100, 100});
                        }
                        break;
                }
            }
            manager.update(tick_timer.get_range() / 1000.0f);
            tick_timer.reload();
            manager.enable_screen_wrapping({wnd.get_width(), wnd.get_height()});
            if(rand.next_int(0, 50) == 2)
                manager.spawn_ghost({rand.next_float(0.0f, 1.0f), rand.next_float(0.0f, 1.0f)}, 0.0f, {100, 100});
        }
        manager.render(&render_shader, &gui_shader, camera, {wnd.get_width(), wnd.get_height()});
        wnd.update(gui_shader, nullptr);
    }
}