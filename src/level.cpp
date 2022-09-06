#include "level.hpp"
#include "images.hpp"
#include "tz/core/assert.hpp"
#include "tz/core/imported_text.hpp"
#include "tz/core/containers/grid_view.hpp"

#include ImportedTextHeader(dev_level_0, png)

namespace game
{
	std::array<std::string_view, static_cast<int>(LevelID::Count)> level_image_data
	{
		ImportedTextData(dev_level_0, png)
	};

	LevelPalette get_level_palette()
	{
		ColourActorMapping actor_map;
		for(int i = 0; i < static_cast<int>(ActorType::Count); i++)
		{
			Actor actor = game::create_actor(static_cast<ActorType>(i));
			const auto col = actor.palette_colour;
			if(col == tz::Vec3ui{0u, 0u, 0u})
			{
				continue;
			}
			const bool exists_already = actor_map.find(col) != actor_map.end();
			tz_assert(!exists_already, "Detected two actor types with identical palette colours that aren't zero. ActorTypes are %d and %d and the palette colour is {%u, %u, %u}", i, static_cast<int>(actor_map[col]), col[0], col[1], col[2]);
			tz_report("actor_map[{%u, %u, %u}] = %d", col[0], col[1], col[2], static_cast<int>(actor.type));
			actor_map[col] = actor.type;
		}
		ColourAttributeMapping attrib_map;
		{
			attrib_map[tz::Vec3ui{64u, 255u, 255u}] = LevelAttribute::PlayerSpawn;
		}
		return
		{
			.actor_palette = actor_map,
			.attribute_palette = attrib_map
		};
	}

	Level load_level(LevelID lid)
	{
		constexpr tz::Vec3ui colour_black{0u, 0u, 0u};
		tz::gl::ImageResource level_image = game::load_image_data(level_image_data[static_cast<int>(lid)]);
		using ImageView = tz::GridView<std::byte, 4>;
		ImageView view{level_image.data(), level_image.get_dimensions()};

		// Firstly figure out which colour is meant to be the player spawn point.
		tz::Vec3ui player_spawn_colour = colour_black;
		LevelPalette palette = get_level_palette();
		for(const auto& [col, attr] : palette.attribute_palette)
		{
			if(attr == LevelAttribute::PlayerSpawn)
			{
				player_spawn_colour = col;
			}
		}
		tz_assert(player_spawn_colour != colour_black, "Player spawn palette colour detected as black. This is not allowed");

		Level ret;

		for(std::size_t x = 0; x < view.get_dimensions()[0]; x++)
		{
			for(std::size_t y = 0; y < view.get_dimensions()[1]; y++)
			{
				auto rgba = view(x, y);
				tz::Vec2 pos = {static_cast<float>(x), static_cast<float>(view.get_dimensions()[1] - y)};
				// TODO: Not a magic number. How much world space should one pixel in the level use?
				pos /= 4.0f;
				pos -= {1.0f, 1.0f};
				tz::Vec4ui colour4
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
					tz_assert(iter != palette.actor_palette.end(), "Level image contained pixel of colour {%u, %u, %u} which does not match any LevelAttribute nor ActorType. Either the image is malformed, incomplete or meant for a different version of RedNightmare");
					ret.actor_spawns.emplace_back(pos, iter->second);
					tz_report("Actor ID %d spawns at {%.2f, %.2f}", iter->second, pos[0], pos[1]);
				}

			}
		}
		return ret;
	}
}
