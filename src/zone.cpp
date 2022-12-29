#include "zone.hpp"

namespace game
{
	Zone get_story_zone(StoryZone zone)
	{
		switch(zone)
		{
			case StoryZone::BlanchfieldGraveyard:
				return
				{
					.initial_spawn = hdk::vec2{15.88f, 7.38f},
					.levels =
					{
						game::load_level(LevelID::DevLevel1),
						game::load_level(LevelID::DevLevel0)
					},
					.name = "Blanchfield Graveyard"
				};
			break;
		}
		hdk::error("Unimplemented story zone");
		return {};
	}
	
	Zone get_procedural_zone(ProceduralZoneInfo pinfo)
	{
		Zone z;
		z.name = "Procedural Zone";
		z.levels.resize(pinfo.level_count);
		RandomLevelGenerationOptions options
		{
			.width = pinfo.level_width,
			.height = pinfo.level_height,
			.empty_chance = pinfo.sparsity,
			.whitelist = pinfo.whitelist,
			.blacklist = pinfo.blacklist,
			.config =
			{
				.actors = [actor_spawns = pinfo.actor_spawn_coefficients]()
				{
					std::unordered_map<ActorType, ActorLayoutConfig> ret;
					for(const auto& [type, coeff] : actor_spawns)
					{
						ret[type] = {.spawn_coefficient = coeff};
					}
					return ret;
				}(),
				.flags = {LevelLayoutFlag::GenerateBorder}
			}
		};
		for(std::size_t l = 0; l < pinfo.level_count; l++)
		{
			auto cur_options = options;
			cur_options.seed = pinfo.seed + l;
			// Note: We want to amend the options for various settings. Namely, we don't want to spawn upwards stairs on the first level, and no downwards stairs on the last one either, etc... We do that now.
			if(l == 0)
			{
				for(ActorType a = static_cast<ActorType>(0); static_cast<int>(a) < static_cast<int>(ActorType::Count); a = static_cast<ActorType>(static_cast<int>(a) + 1))
				{
					// Add all actor types to blacklist that 'go up a level' because you can't do that on the first level.
					if(game::create_actor(a).flags.has<FlagID::GoesUpALevel>())
					{
						cur_options.blacklist |= a;
						cur_options.whitelist.remove(a);
					}
				}
			}
			else if(l == pinfo.level_count - 1)
			{
				for(ActorType a = static_cast<ActorType>(0); static_cast<int>(a) < static_cast<int>(ActorType::Count); a = static_cast<ActorType>(static_cast<int>(a) + 1))
				{
					// Add all actor types to blacklist that 'go up a level' because you can't do that on the first level.
					if(game::create_actor(a).flags.has<FlagID::GoesDownALevel>())
					{
						cur_options.blacklist |= a;
						cur_options.whitelist.remove(a);
					}
				}
			}
			// Now simply fill in the level.
			z.levels[l] = game::load_level_from_image(game::random_level_image(cur_options), {.background = TextureID::Dungeon_Floor_Generic_Backdrop}, EffectID::None);
		}
		return z;
	}

	std::optional<ProceduralZoneInfo> dbgui_procedural_zones()
	{
		struct BoolProxy{bool b = false;};
		// Static data.
		static int level_count = 4;
		static hdk::vec2i dims = hdk::vec2i::filled(32);
		static int sparsity = 75;

		const LevelPalette palette = game::get_level_palette();
		static std::unordered_map<ActorType, bool> whitelist =
		{
			{ActorType::GhostZombie, true},
			{ActorType::GhostBanshee, true},
			{ActorType::Scenery_Gravestone, true},
			{ActorType::WallDestructible, true},
			{ActorType::Wall, true},
			{ActorType::Interactable_Stone_Stairs_Down_NX, true},
			{ActorType::Interactable_Stone_Stairs_Up_NX, true}
		};
		static std::unordered_map<ActorType, bool> blacklist;
		static std::unordered_map<ActorType, float> spawn_coefficients =
		{
			{ActorType::GhostZombie, 0.4f},
			{ActorType::GhostBanshee, 0.04f},
			{ActorType::Scenery_Gravestone, 0.3f},
			{ActorType::Interactable_Stone_Stairs_Down_NX, 0.04f},
			{ActorType::Interactable_Stone_Stairs_Up_NX, 0.02f},
		};
		// Dbgui begin.
		ImGui::SliderInt("Level Count", &level_count, 1, 64);
		ImGui::SliderInt2("Dimensions", dims.data().data(), 4, 64);
		ImGui::SliderInt("Sparsity %", &sparsity, 0, 100);
		if(ImGui::CollapsingHeader("Black/Whitelist"))
		{
			ImGui::BeginTable("Actor Availability", 3, ImGuiTableFlags_Borders);
			ImGui::TableNextRow();
			std::size_t i = 0;
			for(const auto& [_, actor] : palette.actor_palette)
			{
				ImGui::PushID(i++);
				ImGui::TableNextColumn();
				ImGui::Text("%s", game::create_actor(actor).name);
				ImGui::TableNextColumn();
				ImGui::Checkbox("Whitelist", &whitelist[actor]);
				ImGui::TableNextColumn();
				ImGui::Checkbox("Blacklist", &blacklist[actor]);
				ImGui::PopID();
			}
			ImGui::EndTable();
		}

		if(ImGui::Button("Load Zone"))
		{
			auto to_enum_field = [](const std::unordered_map<ActorType, bool>& umap) -> ActorTypes
			{
				ActorTypes ret;
				for(const auto& [a, enabled] : umap)
				{
					if(enabled) ret |= a;	
				}
				return ret;
			};
			return ProceduralZoneInfo
			{
				.seed = 0u,
				.level_count = static_cast<std::size_t>(level_count),
				.level_width = static_cast<unsigned int>(dims[0]),
				.level_height = static_cast<unsigned int>(dims[1]),
				.sparsity = static_cast<unsigned int>(sparsity),
				.whitelist = to_enum_field(whitelist),
				.blacklist = to_enum_field(blacklist),
				.actor_spawn_coefficients = spawn_coefficients
			};
		}
		return std::nullopt;
	}
}
