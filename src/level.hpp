#ifndef REDNIGHTMARE_LEVEL_HPP
#define REDNIGHTMARE_LEVEL_HPP
#include "actor.hpp"
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
	
	using ColourActorMapping = std::unordered_map<hdk::vec3ui, ActorType>;
	using ColourAttributeMapping = std::unordered_map<hdk::vec3ui, LevelAttribute>;

	struct LevelPalette
	{
		ColourActorMapping actor_palette = {};
		ColourAttributeMapping attribute_palette = {};
	};

	LevelPalette get_level_palette();

	enum class LevelID
	{
		Empty,
		DevLevel0,
		DevLevel1,
		DevLevel2,
		Count
	};

	struct Level
	{
		hdk::vec2 player_spawn_location = {0.0f, 0.0f};
		std::vector<std::pair<hdk::vec2, ActorType>> actor_spawns = {};
		hdk::vec2 max_level_coords = hdk::vec2::zero();
		TextureID backdrop;
	};

	enum class LevelLayoutFlag
	{
		GenerateBorder,
		Count
	};
	using LevelLayoutFlags = tz::EnumField<LevelLayoutFlag>;
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
	Level load_level_from_image(const tz::gl::ImageResource& level_image, TextureID backdrop = TextureID::Invisible);

	tz::gl::ImageResource random_level_image(const RandomLevelGenerationOptions& options);
	std::optional<tz::gl::ImageResource> dbgui_generate_random_level_image();
}

#endif // REDNIGHTMARE_LEVEL_HPP
