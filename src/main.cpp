#include "tz/tz.hpp"
#include "tz/core/time.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "gamelib/game.hpp"
#include "gamelib/version.hpp"
#include "gamelib/render/quad_renderer.hpp"

int main()
{
	tz::initialise({.name = "Red Nightmare", .version = rnlib::get_version()});
	rnlib::initialise();
	{

		using namespace tz::literals;
		tz::delay fixed_update = 16670_us;
		while(!tz::window().is_close_requested())
		{
			tz::begin_frame();
			if(fixed_update.done())
			{
				rnlib::update(fixed_update.elapsed().millis<float>());
				fixed_update.reset();
			}
			rnlib::render();
			tz::dbgui::run([]()
			{
				rnlib::dbgui();
			});
			tz::end_frame();
		}
	}
	rnlib::terminate();
	tz::terminate();
}
