#ifndef REDNIGHTMARE_SRC_ZONE_HPP
#define REDNIGHTMARE_SRC_ZONE_HPP
#include "level.hpp"
#include <optional>

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
		tz::vec2 initial_spawn = tz::vec2::zero();
		std::vector<Level> levels = {};
		const char* name = "Unknown";
	};

	enum class ZoneBiome
	{
		Grassy,
		Snowy,
		Dungeon,
		Count
	};

	struct ProceduralZoneIntroLevel
	{
		ZoneBiome biome;
	};

	struct ProceduralZoneInfo
	{
		std::size_t seed = 0u;
		std::size_t level_count;
		unsigned int level_width;
		unsigned int level_height;
		unsigned int sparsity = 50;
		float ambient_lighting = 1.0f;
		ActorTypes whitelist = {};
		ActorTypes blacklist = {};
		std::optional<ProceduralZoneIntroLevel> intro_level = std::nullopt;
		std::unordered_map<ActorType, float> actor_spawn_coefficients = {};
	};

	Zone get_story_zone(StoryZone zone);
	Zone get_procedural_zone(ProceduralZoneInfo pinfo);
	std::optional<ProceduralZoneInfo> dbgui_procedural_zones();
}

#endif // REDNIGHTMARE_SRC_ZONE_HPP
