#include "game.hpp"
#include "effect.hpp"
#include "tz/tz.hpp"

namespace game
{
	void initialise()
	{
		tz::initialise({.name = "Red Nightmare", .version = {2, 1, 0},
			.flags = {tz::application_flag::window_transparent}});
		effects_impl::initialise();
	}

	void terminate()
	{
		effects_impl::terminate();
		tz::terminate();
	}
}
