#ifndef REDNIGHTMARE_SRC_ZONE_HPP
#define REDNIGHTMARE_SRC_ZONE_HPP
#include "level.hpp"

namespace game
{
	enum class StoryZone
	{
		BlanchfieldGraveyard,
	};

	struct Zone
	{
		std::size_t level_cursor = 0;
		std::vector<Level> levels = {};
	};

	Zone get_story_zone(StoryZone zone);
}

#endif // REDNIGHTMARE_SRC_ZONE_HPP
