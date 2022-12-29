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
		bool initial_load = true;
		hdk::vec2 initial_spawn = hdk::vec2::zero();
		std::vector<Level> levels = {};
		const char* name = "Unknown";
	};

	Zone get_story_zone(StoryZone zone);
}

#endif // REDNIGHTMARE_SRC_ZONE_HPP
