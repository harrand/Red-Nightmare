#ifndef REDNIGHTMARE_LEVEL_HPP
#define REDNIGHTMARE_LEVEL_HPP
#include "core/actor.hpp"
#include "gfx/effect.hpp"
#include "gfx/backdrop.hpp"
#include "tz/gl/resource.hpp"
#include <unordered_map>
#include <utility>
#include <random>
#include <unordered_map>

namespace game
{
	/// Describes something in a level that isn't an actor.
	enum class LevelAttribute
	{
		PlayerSpawn
	};
	
	using ColourActorMapping = std::unordered_map<tz::vec3ui, ActorType>;
	using ColourAttributeMapping = std::unordered_map<tz::vec3ui, LevelAttribute>;

	struct LevelPalette
	{
		ColourActorMapping actor_palette = {};
		ColourAttributeMapping attribute_palette = {};
	};

	LevelPalette get_level_palette();

	enum class LevelID
	{
		Empty,
		ProceduralIntro,
		DevLevel0,
		DevLevel1,
		DevLevel2,
		DevLevel3,
		BansheeBossBattleRoom,
		Count
	};

	struct Level
	{
		tz::vec2 player_spawn_location = {2.0f, 2.0f};
		std::vector<std::pair<tz::vec2, ActorType>> actor_spawns = {};
		tz::vec2 max_level_coords = tz::vec2::zero();
		Backdrop backdrop;
		EffectID weather_effect = EffectID::None;
		float ambient_lighting = 1.0f;
	};

	enum class LevelLayoutFlag
	{
		GenerateBorder,
		Count
	};
	using LevelLayoutFlags = tz::enum_field<LevelLayoutFlag>;
	constexpr std::array<const char*, static_cast<std::size_t>(LevelLayoutFlag::Count)> level_layout_flag_names =
	{
		"Generate Border",
	};

	struct ActorLayoutConfig
	{
		float spawn_coefficient = 1.0f;
	};

	struct LevelLayoutConfig
	{
		std::unordered_map<ActorType, ActorLayoutConfig> actors = {};
		LevelLayoutFlags flags = {};
	};

	struct RandomLevelGenerationOptions
	{
		// Number of pixels in each row of the generated level image.
		unsigned int width;
		/// Number of rows in the generated level image.
		unsigned int height;
		std::size_t seed = 0u;
		/// Probability of a single pixel being empty space.
		unsigned int empty_chance = 50;
		ActorTypes whitelist = {};
		ActorTypes blacklist = {};
		LevelLayoutConfig config = {};
	};

	struct RandomLevelInfo
	{
		RandomLevelGenerationOptions gen_options;
		std::default_random_engine rng;
	};

	Level load_level(LevelID lid);
	Level load_level_from_image(const tz::gl::image_resource& level_image, Backdrop backdrop = {}, EffectID weather_effect = EffectID::None, float ambient_lighting = 1.0f);

	tz::gl::image_resource random_level_image(const RandomLevelGenerationOptions& options);
	void impl_enact_blackwhitelists(const ActorTypes& blacklist, const ActorTypes& whitelist, tz::gl::image_resource& level_image);
	struct RandomLevelData
	{
		tz::gl::image_resource level_image;
		Backdrop backdrop = {};
		EffectID weather_effect = EffectID::None;
	};
	std::optional<RandomLevelData> dbgui_generate_random_level_image();
}

#endif // REDNIGHTMARE_LEVEL_HPP
