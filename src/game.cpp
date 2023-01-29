#include "game.hpp"
#include "effect.hpp"
#include "tz/tz.hpp"

namespace game
{
	void initialise()
	{
		tz::initialise({
			.name = "Red Nightmare",
			.version = game::get_version(),
			.flags = {tz::application_flag::window_transparent}});
		effects_impl::initialise();
	}

	void terminate()
	{
		effects_impl::terminate();
		tz::terminate();
	}

	tz::version get_version()
	{
		return tz::version::from_binary_string(GAME_VERSION);
	}
}
