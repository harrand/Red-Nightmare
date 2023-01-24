#include "level.hpp"
#include "images.hpp"
#include "tz/core/imported_text.hpp"
#include "tz/core/data/grid_view.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "tz/core/debug.hpp"

#include ImportedTextHeader(invisible, png)
#include ImportedTextHeader(procedural_intro, png)
#include ImportedTextHeader(dev_level_0, png)
#include ImportedTextHeader(dev_level_1, png)
#include ImportedTextHeader(dev_level_2, png)
#include ImportedTextHeader(dev_level_3, png)
#include ImportedTextHeader(banshee_boss_battle_room, png)

namespace game
{
	constexpr float world_scale = 4.0f;
	std::array<std::string_view, static_cast<int>(LevelID::Count)> level_image_data
	{
		ImportedTextData(invisible, png),
		ImportedTextData(procedural_intro, png),
		ImportedTextData(dev_level_0, png),
		ImportedTextData(dev_level_1, png),
		ImportedTextData(dev_level_2, png),
		ImportedTextData(dev_level_3, png),
		ImportedTextData(banshee_boss_battle_room, png),
	};

	LevelPalette get_level_palette()
	{
		ColourActorMapping actor_map;
		for(int i = 0; i < static_cast<int>(ActorType::Count); i++)
		{
			Actor actor = game::create_actor(static_cast<ActorType>(i));
			const auto col = actor.palette_colour;
			if(col == tz::vec3ui{0u, 0u, 0u})
			{
				continue;
			}
			const bool exists_already = actor_map.find(col) != actor_map.end();
			tz::assert(!exists_already, "Detected two actor types with identical palette colours that aren't zero. ActorTypes are %d and %d and the palette colour is {%u, %u, %u}", i, static_cast<int>(actor_map[col]), col[0], col[1], col[2]);
			actor_map[col] = actor.type;
		}
		ColourAttributeMapping attrib_map;
		{
			attrib_map[tz::vec3ui{64u, 255u, 255u}] = LevelAttribute::PlayerSpawn;
		}
		return
		{
			.actor_palette = actor_map,
			.attribute_palette = attrib_map
		};
	}

	Level load_level(LevelID lid)
	{
		tz::gl::image_resource level_image = game::load_image_data(level_image_data[static_cast<int>(lid)]);
		Backdrop backdrop;
		EffectID weather_effect = EffectID::None;
		float ambient_lighting = 1.0f;
		switch(lid)
		{
			case LevelID::DevLevel0:
				ambient_lighting = 0.4f;
			break;
			case LevelID::DevLevel1:
				backdrop.background = TextureID::Grass_Generic_Backdrop;
				backdrop.foreground = TextureID::DevLevel1_Backdrop;
				weather_effect = EffectID::Rain;
			break;
			case LevelID::DevLevel2:
				backdrop.background = TextureID::Snow_Generic_Backdrop;
				weather_effect = EffectID::Snow;
			break;
			case LevelID::DevLevel3:
				backdrop.background = TextureID::Snow_Generic_Backdrop;
				backdrop.foreground = TextureID::DevLevel1_Backdrop;
				weather_effect = EffectID::Snow;
			break;
			case LevelID::BansheeBossBattleRoom:
				backdrop.background = TextureID::Dungeon_Floor_Generic_Backdrop;
			break;
		}
		return load_level_from_image(level_image, backdrop, weather_effect, ambient_lighting);
	}

	Level load_level_from_image(const tz::gl::image_resource& level_image, Backdrop backdrop, EffectID weather_effect, float ambient_lighting)
	{
		constexpr tz::vec3ui colour_black{0u, 0u, 0u};
		using ImageView = tz::grid_view<const std::byte, 4>;
		ImageView view{level_image.data(), level_image.get_dimensions()};

		// Firstly figure out which colour is meant to be the player spawn point.
		tz::vec3ui player_spawn_colour = colour_black;
		LevelPalette palette = get_level_palette();
		for(const auto& [col, attr] : palette.attribute_palette)
		{
			if(attr == LevelAttribute::PlayerSpawn)
			{
				player_spawn_colour = col;
			}
		}
		tz::assert(player_spawn_colour != colour_black, "Player spawn palette colour detected as black. This is not allowed");

		Level ret;
		ret.backdrop = backdrop;
		ret.weather_effect = weather_effect;
		ret.ambient_lighting = ambient_lighting;
		ret.max_level_coords = static_cast<tz::vec2>(level_image.get_dimensions()) / world_scale;

		for(std::size_t x = 0; x < view.get_dimensions()[0]; x++)
		{
			for(std::size_t y = 0; y < view.get_dimensions()[1]; y++)
			{
				auto rgba = view(x, y);
				tz::vec2 pos = {static_cast<float>(x), static_cast<float>(view.get_dimensions()[1] - y)};
				// Remember: (0, 0) is the middle of the actor, but the corner of the pixel. So we take away (0.5, 0.5) so the actor pos lines up with the pixel (note that y is inverted). Maybe. Perhaps
				pos += tz::vec2{0.5f, -0.5f};
				// Convert to arbitrary world scale (this is probably pointless)
				pos /= world_scale;
				tz::vec4ui colour4
				{
					static_cast<unsigned int>(rgba[0]),
					static_cast<unsigned int>(rgba[1]),
					static_cast<unsigned int>(rgba[2]),
					static_cast<unsigned int>(rgba[3])
				};
				if(colour4[3] == 0.0f)
				{
					// transparent pixel. ignore.
					continue;
				}
				auto colour = colour4.swizzle<0, 1, 2>();
				// Colour *must* match either an actor or level attribute colour, or the level image is malformed.
				if(colour == player_spawn_colour)
				{
					ret.player_spawn_location = pos;
				}
				else
				{
					// It must be an actor.
					auto iter = palette.actor_palette.find(colour);
					tz::assert(iter != palette.actor_palette.end(), "Level image contained pixel of colour {%u, %u, %u} which does not match any LevelAttribute nor ActorType. Either the image is malformed, incomplete or meant for a different version of RedNightmare");
					ret.actor_spawns.emplace_back(pos, iter->second);
				}

			}
		}
		return ret;
	}

	tz::gl::image_resource random_level_image(const RandomLevelGenerationOptions& options)
	{
		constexpr tz::vec3ui colour_black{0u, 0u, 0u};
		RandomLevelInfo info
		{
			.gen_options = options,
		};

		info.rng.seed(tz::system_time().millis<unsigned int>());

		std::vector<std::byte> image_data;
		image_data.resize(4 * info.gen_options.width * info.gen_options.height, std::byte{0});

		tz::vec2ui dims{info.gen_options.width, info.gen_options.height};
		using ImageView = tz::grid_view<std::byte, 4>;
		ImageView view{image_data, dims};

		auto colvec_to_bytes = [](std::span<std::byte> bytes, tz::vec3ui col)
		{
			bytes[0] = static_cast<std::byte>(col[0]);
			bytes[1] = static_cast<std::byte>(col[1]);
			bytes[2] = static_cast<std::byte>(col[2]);
			bytes[3] = static_cast<std::byte>(1.0f);
		};
		std::vector<tz::vec3ui> actor_pool;
		auto add_to_pool = [&info, &actor_pool](ActorType t)
		{
			for(float f = 0; f < info.gen_options.config.actors[t].spawn_coefficient; f += 0.01f)
			{
				actor_pool.push_back(game::create_actor(t).palette_colour);			
			}
		};
		// If we have a whitelist, can only be things from the whitelist.
		if(!info.gen_options.whitelist.empty())
		{
			for(ActorType t : info.gen_options.whitelist)
			{
				add_to_pool(t);
			}
		}
		else
		{
			// If no whitelist, can be anything that has a palette colour and isn't in the blacklist.
			for(int i = 0; i < static_cast<int>(ActorType::Count); i++)
			{
				ActorType t = static_cast<ActorType>(i);
				Actor a = game::create_actor(t);
				if(a.palette_colour != colour_black && !info.gen_options.blacklist.contains(t))
				{
					add_to_pool(t);
				}
			}
		}

		if(!actor_pool.empty())
		{
			std::uniform_int_distribution<unsigned int> dist_pct{0, 100};
			std::uniform_int_distribution<unsigned int> dist_actor_colour{0, static_cast<unsigned int>(actor_pool.size() - 1)};

			for(std::size_t x = 0; x < dims[0]; x++)
			{
				for(std::size_t y = 0; y < dims[1]; y++)
				{
					std::span<std::byte> pixel = view(x, y);
					if(dist_pct(info.rng) < info.gen_options.empty_chance)
					{
						continue;
					}
					tz::vec3ui desired_colour = actor_pool[dist_actor_colour(info.rng)];
					colvec_to_bytes(pixel, desired_colour);
				}
			}
		}
	
		if(options.config.flags.contains(LevelLayoutFlag::GenerateBorder))
		{
			const tz::vec3 wall_colour = game::create_actor(ActorType::Wall).palette_colour;
			for(std::size_t x = 0; x < dims[0]; x++)
			{
				for(std::size_t y = 0; y < dims[1]; y++)
				{
					if((x == 0 || x == (dims[0] - 1)) || (y == 0 || y == (dims[1] - 1)))
					{
						colvec_to_bytes(view(x, y), wall_colour);
					}
				}
			}
		}

		tz::gl::image_resource res = tz::gl::image_resource::from_memory(image_data,
		{
			.format = tz::gl::image_format::RGBA32,
			.dimensions = dims
		});
		return res;
	}

	void impl_enact_blackwhitelists(const ActorTypes& blacklist, const ActorTypes& whitelist, tz::gl::image_resource& level_image)
	{
		using ImageView = tz::grid_view<std::byte, 4>;
		ImageView view{level_image.data(), level_image.get_dimensions()};
		for(std::size_t x = 0; x < view.get_dimensions()[0]; x++)
		{
			for(std::size_t y = 0; y < view.get_dimensions()[1]; y++)
			{
				auto rgba = view(x, y);
				if(std::any_of(blacklist.begin(), blacklist.end(), [&rgba](ActorType t)
				{
					tz::vec3ui rgba_as_vec
					{
						static_cast<unsigned int>(rgba[0]),
						static_cast<unsigned int>(rgba[1]),
						static_cast<unsigned int>(rgba[2])
					};
					return (rgba_as_vec == game::create_actor(t).palette_colour);
				}) ||
				(!whitelist.empty() && std::none_of(whitelist.begin(), whitelist.end(), [&rgba](ActorType t)
				{
					tz::vec3ui rgba_as_vec
					{
						static_cast<unsigned int>(rgba[0]),
						static_cast<unsigned int>(rgba[1]),
						static_cast<unsigned int>(rgba[2])
					};
					return (rgba_as_vec == game::create_actor(t).palette_colour);
				})))
				{
					rgba[0] = std::byte{255};
					rgba[1] = std::byte{0};
					rgba[2] = std::byte{0};
					rgba[3] = std::byte{0};
				}
			}
		}
	}
}
