#include "game.hpp"
#include "tz/core/tz.hpp"

namespace game
{
	void initialise()
	{
		tz::initialise({.name = "Red Nightmare", .version = {2, 1, 0}});
	}

	void terminate()
	{
		tz::terminate();
	}
}
