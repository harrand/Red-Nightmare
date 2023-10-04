#include "gamelib/rnlib.hpp"
#include "tz/tz.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "tz/core/profile.hpp"

struct dbgui_data_t
{
	bool enabled = false;
} dbgui_data;

int main()
{
	tz::initialise
	({
		.name = "Red Nightmare",
		.version = {5u, 0u, 0u, tz::version_type::alpha}
	});
	{
		tz::dbgui::game_menu().add_callback(game::dbgui);
		tz::dbgui::game_bar().add_callback(game::dbgui_game_bar);

		game::initialise();
		while(!tz::window().is_close_requested())
		{
			TZ_FRAME_BEGIN;
			tz::begin_frame();
			tz::dbgui::run([]()
			{
				if(dbgui_data.enabled)
				{
					game::dbgui();
				}
			});
			game::update();
			tz::end_frame();
			TZ_FRAME_END;
		}
		game::terminate();
	}
	tz::terminate();
}