#include "gamelib/rnlib.hpp"
#include "gamelib/config.hpp"
#include "tz/tz.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "tz/core/profile.hpp"
#include "tz/core/time.hpp"

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
		tz::duration t = tz::system_time();
		tz::duration fixed_update = tz::system_time();

		game::initialise();
		while(!tz::window().is_close_requested())
		{
			TZ_FRAME_BEGIN;
			tz::begin_frame();
			auto diff = (tz::system_time() - t).micros<std::uint64_t>();
			t = tz::system_time();
			game::update(diff);
			auto fdiff = (tz::system_time() - fixed_update).micros<std::uint64_t>();
			constexpr std::uint64_t fixed_period_micros = 1000000 / game::config::tick_rate;
			if(fdiff >= fixed_period_micros)
			{
				game::fixed_update(fdiff, fdiff - fixed_period_micros);
				fixed_update = t;
			}
			tz::end_frame();
			TZ_FRAME_END;
		}
		game::terminate();
	}
	tz::terminate();
}