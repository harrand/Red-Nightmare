#ifndef REDNIGHTMARE_LEVEL_HPP
#define REDNIGHTMARE_LEVEL_HPP
#include "actor.hpp"
#include <unordered_map>
#include <utility>

namespace game
{
	/// Describes something in a level that isn't an actor.
	enum class LevelAttribute
	{
		PlayerSpawn
	};
	
	using ColourActorMapping = std::unordered_map<tz::Vec3ui, ActorType>;
	using ColourAttributeMapping = std::unordered_map<tz::Vec3ui, LevelAttribute>;

	struct LevelPalette
	{
		ColourActorMapping actor_palette = {};
		ColourAttributeMapping attribute_palette = {};
	};

	LevelPalette get_level_palette();

	enum class LevelID
	{
		DevLevel0,
		Count
	};

	struct Level
	{
		tz::Vec2 player_spawn_location = {0.0f, 0.0f};
		std::vector<std::pair<tz::Vec2, ActorType>> actor_spawns = {};
	};

	Level load_level(LevelID lid);
}

#endif // REDNIGHTMARE_LEVEL_HPP
