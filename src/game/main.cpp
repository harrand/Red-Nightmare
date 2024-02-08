#include "gamelib/rnlib.hpp"
#include "gamelib/config.hpp"
#include "tz/tz.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "tz/core/profile.hpp"
#include "tz/core/time.hpp"

#include <iostream>

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
		tz::duration fps_limiter = tz::system_time();

		game::initialise();

		tz::lua::get_state().execute(R"(
			rn.level.load("startscreen")
			--for i=1,64,1 do
			--	rn.current_scene():add_entity("morbius")
			--end
		)");

		while(!tz::window().is_close_requested())
		{
			TZ_FRAME_BEGIN;
			fps_limiter = tz::system_time();
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
			const std::uint64_t millis_this_frame = (tz::system_time() - fps_limiter).millis<std::uint64_t>();
			constexpr std::uint64_t min_millis_this_frame = 1000 / 120;
			constexpr bool fps_limit_enabled = false;
			if(fps_limit_enabled && !TZ_PROFILE && millis_this_frame < min_millis_this_frame)
			{
				TZ_PROFZONE("FPS Limit Wait", 0xFF333333);
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(std::chrono::duration<std::uint64_t, std::milli>{min_millis_this_frame - millis_this_frame});
			}
			TZ_FRAME_END;
		}
		game::terminate();
	}
	tz::terminate();
}
