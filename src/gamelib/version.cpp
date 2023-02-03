#include "gamelib/version.hpp"

namespace rnlib
{
	tz::version get_version()
	{
		return tz::version::from_binary_string(GAME_VERSION);
	}
}
