#include "level.hpp"
#include "images.hpp"
#include "tz/core/imported_text.hpp"
#include "tz/core/containers/grid_view.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "hdk/debug.hpp"

#include ImportedTextHeader(invisible, png)
#include ImportedTextHeader(dev_level_0, png)
#include ImportedTextHeader(dev_level_1, png)
#include ImportedTextHeader(dev_level_2, png)

namespace game
{
	constexpr float world_scale = 4.0f;
	std::array<std::string_view, static_cast<int>(LevelID::Count)> level_image_data
	{
		ImportedTextData(invisible, png),
		ImportedTextData(dev_level_0, png),
		ImportedTextData(dev_level_1, png),
		ImportedTextData(dev_level_2, png)
	};

	LevelPalette get_level_palette()
	{
		ColourActorMapping actor_map;
		for(int i = 0; i < static_cast<int>(ActorType::Count); i++)
		{
			Actor actor = game::create_actor(static_cast<ActorType>(i));
			const auto col = actor.palette_colour;
			if(col == hdk::vec3ui{0u, 0u, 0u})
			{
				continue;
			}
			const bool exists_already = actor_map.find(col) != actor_map.end();
			hdk::assert(!exists_already, "Detected two actor types with identical palette colours that aren't zero. ActorTypes are %d and %d and the palette colour is {%u, %u, %u}", i, static_cast<int>(actor_map[col]), col[0], col[1], col[2]);
			actor_map[col] = actor.type;
		}
		ColourAttributeMapping attrib_map;
		{
			attrib_map[hdk::vec3ui{64u, 255u, 255u}] = LevelAttribute::PlayerSpawn;
		}
		return
		{
			.actor_palette = actor_map,
			.attribute_palette = attrib_map
		};
	}

	Level load_level(LevelID lid)
	{
		tz::gl::ImageResource level_image = game::load_image_data(level_image_data[static_cast<int>(lid)]);
		TextureID backdrop = TextureID::Invisible;
		switch(lid)
		{
			case LevelID::DevLevel1:
				backdrop = TextureID::DevLevel1_Backdrop;
			break;
		}
		return load_level_from_image(level_image, backdrop);
	}

	Level load_level_from_image(const tz::gl::ImageResource& level_image, TextureID backdrop)
	{
		constexpr hdk::vec3ui colour_black{0u, 0u, 0u};
		using ImageView = tz::GridView<const std::byte, 4>;
		ImageView view{level_image.data(), level_image.get_dimensions()};

		// Firstly figure out which colour is meant to be the player spawn point.
		hdk::vec3ui player_spawn_colour = colour_black;
		LevelPalette palette = get_level_palette();
		for(const auto& [col, attr] : palette.attribute_palette)
		{
			if(attr == LevelAttribute::PlayerSpawn)
			{
				player_spawn_colour = col;
			}
		}
		hdk::assert(player_spawn_colour != colour_black, "Player spawn palette colour detected as black. This is not allowed");

		Level ret;
		ret.backdrop = backdrop;
		ret.max_level_coords = static_cast<hdk::vec2>(level_image.get_dimensions()) / world_scale;

		for(std::size_t x = 0; x < view.get_dimensions()[0]; x++)
		{
			for(std::size_t y = 0; y < view.get_dimensions()[1]; y++)
			{
				auto rgba = view(x, y);
				hdk::vec2 pos = {static_cast<float>(x), static_cast<float>(view.get_dimensions()[1] - y)};
				// TODO: Not a magic number. How much world space should one pixel in the level use?
				pos /= world_scale;
				pos -= {1.0f, 1.0f};
				hdk::vec4ui colour4
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
					hdk::assert(iter != palette.actor_palette.end(), "Level image contained pixel of colour {%u, %u, %u} which does not match any LevelAttribute nor ActorType. Either the image is malformed, incomplete or meant for a different version of RedNightmare");
					ret.actor_spawns.emplace_back(pos, iter->second);
				}

			}
		}
		return ret;
	}

	tz::gl::ImageResource random_level_image(const RandomLevelGenerationOptions& options)
	{
		constexpr hdk::vec3ui colour_black{0u, 0u, 0u};
		RandomLevelInfo info
		{
			.gen_options = options,
		};

		info.rng.seed(tz::system_time().millis<unsigned int>());

		std::vector<std::byte> image_data;
		image_data.resize(4 * info.gen_options.width * info.gen_options.height, std::byte{0});

		hdk::vec2ui dims{info.gen_options.width, info.gen_options.height};
		using ImageView = tz::GridView<std::byte, 4>;
		ImageView view{image_data, dims};

		auto colvec_to_bytes = [](std::span<std::byte> bytes, hdk::vec3ui col)
		{
			bytes[0] = static_cast<std::byte>(col[0]);
			bytes[1] = static_cast<std::byte>(col[1]);
			bytes[2] = static_cast<std::byte>(col[2]);
			bytes[3] = static_cast<std::byte>(1.0f);
		};
		std::vector<hdk::vec3ui> actor_pool;
		// If we have a whitelist, can only be things from the whitelist.
		if(!info.gen_options.whitelist.empty())
		{
			for(ActorType t : info.gen_options.whitelist)
			{
				actor_pool.push_back(game::create_actor(t).palette_colour);
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
					actor_pool.push_back(a.palette_colour);
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
					hdk::vec3ui desired_colour = actor_pool[dist_actor_colour(info.rng)];
					colvec_to_bytes(pixel, desired_colour);
				}
			}
		}
	
		if(options.config.flags.contains(LevelLayoutFlag::GenerateBorder))
		{
			const hdk::vec3 wall_colour = game::create_actor(ActorType::Wall).palette_colour;
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

		tz::gl::ImageResource res = tz::gl::ImageResource::from_memory(image_data,
		{
			.format = tz::gl::ImageFormat::RGBA32,
			.dimensions = dims
		});
		return res;
	}

	std::optional<tz::gl::ImageResource> dbgui_generate_random_level_image()
	{
		struct BoolProxy{bool b = false;};
		static std::vector<BoolProxy> level_layout_flag_values(static_cast<std::size_t>(LevelLayoutFlag::Count));
		const LevelPalette palette = get_level_palette();
		static std::unordered_map<ActorType, bool> whitelist_actors(palette.actor_palette.size());
		static std::unordered_map<ActorType, bool> blacklist_actors(palette.actor_palette.size());
		if(ImGui::CollapsingHeader("Black/White Listing", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::BeginTable("Black/White Listing", 3, ImGuiTableFlags_Borders);
			ImGui::TableNextRow();
			std::size_t i = 0;
			for(const auto&[_, actor] : palette.actor_palette)
			{
				ImGui::PushID(i++);
				ImGui::TableNextColumn();
				ImGui::Text(game::create_actor(actor).name);
				ImGui::TableNextColumn();
				ImGui::Checkbox("Whitelist", &whitelist_actors[actor]);
				ImGui::TableNextColumn();
				ImGui::Checkbox("Blacklist", &blacklist_actors[actor]);
				ImGui::PopID();
			}
			ImGui::EndTable();
		}
		for(std::size_t i = 0; i < static_cast<std::size_t>(LevelLayoutFlag::Count); i++)
		{
			ImGui::Checkbox(level_layout_flag_names[i], &level_layout_flag_values[i].b);
		}
		if(ImGui::Button("Generate"))
		{
			ActorTypes whitelist;
			ActorTypes blacklist;
			for(const auto&[actor, b] : whitelist_actors)
			{
				if(b){whitelist |= actor;}
			}
			for(const auto&[actor, b] : blacklist_actors)
			{
				if(b){blacklist |= actor;}
			}
			LevelLayoutFlags flags;
			for(std::size_t i = 0; i < level_layout_flag_values.size(); i++)
			{
				if(level_layout_flag_values[i].b)
				{
					flags |= static_cast<LevelLayoutFlag>(i);
				}
			}
			return random_level_image
			({
				.width = 32,
				.height = 32,
				.seed = 32u,
				.whitelist = whitelist,
				.blacklist = blacklist,
				.config =
				{
					.flags = flags
				}
			});
		}
		return {};
	}
}
