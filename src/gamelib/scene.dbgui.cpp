#include "gamelib/scene.hpp"
#include "gamelib/render/scene_renderer.hpp"
#include "gamelib/messaging/scene.hpp"
#include "gamelib/lua/meta.hpp"
#include "imgui.h"

namespace game
{
	void dbgui_ent_ren(const render::scene_renderer::entry& ren, const render::scene_renderer& renderer)
	{
		ImGui::Text("Model: %s", ren.obj == tz::nullhand ? "None" : ren.model_name.c_str());
		if(ren.obj != tz::nullhand)
		{
			ImGui::Text("AOH: %zu", static_cast<std::size_t>(static_cast<tz::hanval>(ren.obj)));
			tz::ren::animation_renderer::gltf_handle gltfh = renderer.get_renderer().animated_object_get_gltf(ren.obj);
			ImGui::Text("GLTFH: %zu", static_cast<std::size_t>(static_cast<tz::hanval>(gltfh)));
		}
	}

	void dbgui_ent(const scene_entity_data& edata, const render::scene_renderer& renderer)
	{
		std::string tree_title = std::format("{}: {}", edata.ent.uuid, edata.ent.name);
		bool any_messages_sent = false;
		bool deleted = false;
		if(ImGui::Button("x"))
		{
			any_messages_sent = true;
			game::messaging::scene_insert_message
			({
				.operation = game::messaging::scene_operation::remove_entity,
				.uuid = edata.ent.uuid
			});
			deleted = true;
		}
		ImGui::SameLine();
		if(!deleted && ImGui::TreeNode(tree_title.c_str()))
		{
			dbgui_ent_ren(edata.ren, renderer);
			std::string subtree_title = std::format("Internals ({})", edata.ent.internal_variables.size());
			if(edata.ent.internal_variables.size() && ImGui::TreeNode(subtree_title.c_str()))
			{
				ImGui::Separator();
				for(const auto& [name, val] : edata.ent.internal_variables)
				{
					std::string button_label = "x##" + name + std::to_string(edata.ent.uuid);
					if(ImGui::Button(button_label.c_str()))
					{
						game::messaging::scene_insert_message({
							.operation = game::messaging::scene_operation::entity_write,
							.uuid = edata.ent.uuid,
							.value = std::pair<std::string, tz::lua::lua_generic>{name, tz::lua::nil{}}
						});
						any_messages_sent = true;
					}
					ImGui::SameLine();
					ImGui::Text("%s", name.c_str());
					float text_width_pixels = ImGui::CalcTextSize(name.c_str()).x;
					ImGui::SameLine();
					// we want this to take the same amount of horizontal space no matter the text size.
					// say 100.
					const float margin_size = ImGui::GetWindowContentRegionWidth() / 5.0f;
					if(text_width_pixels < margin_size)
					{
						ImGui::Dummy(ImVec2{margin_size - text_width_pixels, 0.0f});
						ImGui::SameLine();
					}
					std::visit([name, uuid = edata.ent.uuid, &any_messages_sent](auto&& arg)
					{
						using T = std::decay_t<decltype(arg)>;
						std::string label = "##" + name + std::to_string(uuid);
						if constexpr(std::is_same_v<T, bool>)
						{
							bool b = arg;
							if(ImGui::Checkbox(label.c_str(), &b))
							{
								game::messaging::scene_insert_message({
									.operation = game::messaging::scene_operation::entity_write,
									.uuid = uuid,
									.value = std::pair<std::string, tz::lua::lua_generic>{name, b}
								});
								any_messages_sent = true;
							}
						}
						else if constexpr(std::is_same_v<T, std::string>)
						{
							std::string s = arg;
							if(ImGui::InputText(label.c_str(), &s, ImGuiInputTextFlags_EnterReturnsTrue))
							{
								game::messaging::scene_insert_message({
									.operation = game::messaging::scene_operation::entity_write,
									.uuid = uuid,
									.value = std::pair<std::string, tz::lua::lua_generic>{name, s}
								});
								any_messages_sent = true;
							}
						}
						else if constexpr(std::is_same_v<T, tz::lua::nil>)
						{
							(void)arg;
							ImGui::Text("nil");
							ImGui::SameLine();
							ImGui::Dummy(ImVec2{25, 0});
							ImGui::SameLine();
							ImGui::Text("Set as:");
							ImGui::SameLine();
							// buttons to initialise to any value type.
							tz::lua::lua_generic new_value = tz::lua::nil{};
							bool reinitialise = false;
							std::string bool_label = "bool" + label;
							if(ImGui::Button(bool_label.c_str()))
							{
								new_value = false;
								reinitialise = true;
							}
							ImGui::SameLine();
							std::string double_label = "double" + label;
							if(ImGui::Button(double_label.c_str()))
							{
								new_value = 0.0;
								reinitialise = true;
							}
							ImGui::SameLine();
							std::string integer_label = "integer" + label;
							if(ImGui::Button(integer_label.c_str()))
							{
								new_value = std::int64_t{0};
								reinitialise = true;
							}
							ImGui::SameLine();
							std::string string_label = "string" + label;
							if(ImGui::Button(string_label.c_str()))
							{
								new_value = std::string{""},
								reinitialise = true;
							}
							if(reinitialise)
							{
								game::messaging::scene_insert_message({
									.operation = game::messaging::scene_operation::entity_write,
									.uuid = uuid,
									.value = std::pair<std::string, tz::lua::lua_generic>{name, new_value}
								});
								any_messages_sent = true;
							}
						}
						else
						{
							// its a number of some kind. specifically, a double or std::int64_t
							if constexpr(std::is_same_v<T, double>)
							{
								double d = arg;
								if(ImGui::InputDouble(label.c_str(), &d, 0.0, 0.0, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue))
								{
									game::messaging::scene_insert_message({
										.operation = game::messaging::scene_operation::entity_write,
										.uuid = uuid,
										.value = std::pair<std::string, tz::lua::lua_generic>{name, d}
									});
									any_messages_sent = true;
								}
							}
							else if constexpr(std::is_same_v<T, std::int64_t>)
							{
								std::int64_t i = arg;
								if(ImGui::InputScalar(label.c_str(), ImGuiDataType_S64, &i, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue))
								{
									game::messaging::scene_insert_message({
										.operation = game::messaging::scene_operation::entity_write,
										.uuid = uuid,
										.value = std::pair<std::string, tz::lua::lua_generic>{name, i}
									});
									any_messages_sent = true;
								}

							}
						}
					}, val);
					ImGui::Separator();
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}	
		if(any_messages_sent)
		{
			// make sure any messages we sent get dispatched instantly.
			// otherwise, stuff that changes this frame could message after us and thus completely drown our edit out.
			game::messaging::scene_messaging_local_dispatch();
			game::messaging::scene_messaging_update();
		}
	}

	void scene::dbgui()
	{
		std::string label = std::format("Entities ({})", this->entity_count());
		if(ImGui::CollapsingHeader(label.c_str()))
		{
			for(const scene_entity_data& edata : *this)
			{
				dbgui_ent(edata, this->get_renderer());
			}
		}
		if(ImGui::CollapsingHeader("Mods"))
		{
			for(std::size_t i = 0; i < game::meta::get_mods().size(); i++)
			{
				const auto& mod = game::meta::get_mods()[i];
				std::vector<meta::prefabinfo_t> owned_prefabs = {};
				std::vector<meta::levelinfo_t> owned_levels = {};
				std::vector<meta::spellinfo_t> owned_spells = {};
				std::vector<meta::iteminfo_t> owned_items = {};
				for(const auto& prefab : game::meta::get_prefabs())
				{
					if(prefab.mod_id == i)
					{
						owned_prefabs.push_back(prefab);
					}
				}
				for(const auto& level : game::meta::get_levels())
				{
					if(level.mod_id == i)
					{
						owned_levels.push_back(level);
					}
				}
				for(const auto& spell : game::meta::get_spells())
				{
					if(spell.mod_id == i)
					{
						owned_spells.push_back(spell);
					}
				}
				for(const auto& item : game::meta::get_items())
				{
					if(item.mod_id == i)
					{
						owned_items.push_back(item);
					}
				}
				ImGui::SetNextItemOpen(mod.name == "basegame");
				if(ImGui::TreeNode(mod.name.c_str()))
				{
					ImGui::Text("Description: %s", mod.description.c_str());
					if(owned_prefabs.size() && ImGui::TreeNode("Prefabs"))
					{
						static bool hide_prefabs_without_pre_instantiate = true;
						static bool hide_prefabs_without_on_collision = true;
						ImGui::Checkbox("Hide Prefabs without Pre-Instantiate", &hide_prefabs_without_pre_instantiate);
						ImGui::Checkbox("Hide Prefabs without On-Collision", &hide_prefabs_without_on_collision);
						for(auto prefab : owned_prefabs)
						{
							if(hide_prefabs_without_pre_instantiate && !prefab.has_pre_instantiate)
							{
								continue;
							}
							if(hide_prefabs_without_on_collision && !prefab.has_on_collision)
							{
								continue;
							}
							if(ImGui::TreeNode(prefab.name.c_str()))
							{
								ImGui::Text("%s", prefab.description.c_str());
								ImGui::BeginDisabled();
								ImGui::Checkbox("Has Static Init", &prefab.has_static_init);
								ImGui::Checkbox("Has Pre Instantiate", &prefab.has_pre_instantiate);
								ImGui::Checkbox("Has Instantiate", &prefab.has_instantiate);
								ImGui::Checkbox("Has Update", &prefab.has_update);
								ImGui::Checkbox("Has On-Collision", &prefab.has_on_collision);
								ImGui::EndDisabled();

								if(ImGui::Button("Add to Current Scene"))
								{
									game::messaging::scene_quick_add(prefab.name);
								}

								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
					if(owned_levels.size() && ImGui::TreeNode("Levels"))
					{
						for(auto level : owned_levels)
						{
							if(ImGui::TreeNode(level.name.c_str()))
							{
								if(ImGui::Button("Load Level"))
								{
									std::string lua_cmd = std::format("rn.level.load(\"{}\")", level.name);
									tz::lua::get_state().execute(lua_cmd.c_str());
								}
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
					if(owned_spells.size() && ImGui::TreeNode("Spells"))
					{
						for(auto spell : owned_spells)
						{
							if(ImGui::TreeNode(spell.name.c_str()))
							{
								ImGui::Text("%s", spell.description.c_str());
								ImGui::Text("Type: %s", spell.magic_type.c_str());
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
					if(owned_items.size() && ImGui::TreeNode("Items"))
					{
						for(auto item : owned_items)
						{
							ImVec4 col{item.rarity_colour[0], item.rarity_colour[1], item.rarity_colour[2], 1.0f};
							ImGui::PushStyleColor(ImGuiCol_Text, col);
							if(ImGui::TreeNode(item.name.c_str()))
							{
								ImGui::Text("%s", item.tooltip.c_str());
								ImGui::Text("Slot ID: %zu", item.slot_id);
								ImGui::Text("Rarity: %s", item.rarity.c_str());
								ImGui::TreePop();
							}
							ImGui::PopStyleColor();
						}
						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
			}
		}
	}

	template<typename T>
	std::optional<T> try_get_internal(const game::entity& ent, std::string internal_name)
	{
		tz::lua::lua_generic maybe_val = ent.get_internal(internal_name);
		if(std::holds_alternative<tz::lua::nil>(maybe_val))
		{
			return std::nullopt;
		}
		if(std::holds_alternative<T>(maybe_val))
		{
			return std::get<T>(maybe_val);
		}
		return std::nullopt;
	}
	
	int evaluate_lua_int(std::string lua_expr)
	{
		std::string code = std::format("internal_result = {}", lua_expr);
		tz::lua::get_state().execute(code.c_str());
		auto maybe_dub = tz::lua::get_state().get_int("internal_result");
		return maybe_dub.value_or(0);
	}

	double evaluate_lua_double(std::string lua_expr)
	{
		std::string code = std::format("internal_result = {}", lua_expr);
		tz::lua::get_state().execute(code.c_str());
		auto maybe_dub = tz::lua::get_state().get_double("internal_result");
		return maybe_dub.value_or(0.0);
	}

	bool evaluate_lua_bool(std::string lua_expr)
	{
		std::string code = std::format("internal_result = {}", lua_expr);
		tz::lua::get_state().execute(code.c_str(), false);
		auto maybe_dub = tz::lua::get_state().get_bool("internal_result");
		return maybe_dub.value_or(false);
	}

	tz::vec3 evaluate_lua_vec3(std::string lua_expr)
	{
		std::string code = std::format("internal_result = {}; internal_result_x = internal_result[1]; internal_result_y = internal_result[2]; internal_result_z = internal_result[3]", lua_expr);
		tz::lua::get_state().execute(code.c_str());
		auto x = tz::lua::get_state().get_float("internal_result_x").value_or(0.0f);
		auto y = tz::lua::get_state().get_float("internal_result_y").value_or(0.0f);
		auto z = tz::lua::get_state().get_float("internal_result_z").value_or(0.0f);
		return {x, y, z};
	}

	void dbgui_combat_ent(scene& sc, entity_uuid uuid)
	{
		game::entity& ent = sc.get_entity(uuid);
		std::string title = std::format("{}: {}", ent.uuid, ent.name);
		ImGui::Text(title.c_str());

		auto max_hp = evaluate_lua_double(std::format("rn.entity.prefabs.combat_stats.get_max_hp({})", uuid));
		auto movement_speed = evaluate_lua_double(std::format("rn.entity.prefabs.combat_stats.get_movement_speed({})", uuid));
		auto alive = evaluate_lua_bool(std::format("rn.entity.prefabs.combat_stats.is_alive({})", uuid));
		auto dead = evaluate_lua_bool(std::format("rn.entity.prefabs.combat_stats.is_dead({})", uuid));
		auto lost_hp = try_get_internal<double>(ent, "hp_lost").value_or(0.0);
		auto invincible = try_get_internal<bool>(ent, "invincible").value_or(false);
		if(max_hp == 0.0)
		{
			ImGui::Text("Not a combatant.");
			return;
		}
		ImGui::Text("Invincible: %s", invincible ? "true" : "false");
		ImGui::Text("Status: %s", alive ? "Alive" : "Dead");
		double current_hp = max_hp - lost_hp;
		tz::assert(current_hp >= 0.0f);
		ImGui::Text("HP: %.1f/%.1f (%.2f%%)", current_hp, max_hp, 100.0 * current_hp / max_hp);

		ImGui::Separator();
		ImGui::Text("Movement Speed: %.1f", movement_speed);
		auto physical_power = evaluate_lua_double(std::format("rn.entity.prefabs.combat_stats.get_physical_power({})", uuid));
		auto fire_power = evaluate_lua_double(std::format("rn.entity.prefabs.combat_stats.get_fire_power({})", uuid));
		auto frost_power = evaluate_lua_double(std::format("rn.entity.prefabs.combat_stats.get_frost_power({})", uuid));
		auto fire_colour = evaluate_lua_vec3(std::format("rn.spell.schools.fire.colour", uuid));
		auto frost_colour = evaluate_lua_vec3(std::format("rn.spell.schools.frost.colour", uuid));

		auto physical_resist = evaluate_lua_double(std::format("rn.entity.prefabs.combat_stats.get_physical_resist({})", uuid));
		auto fire_resist = evaluate_lua_double(std::format("rn.entity.prefabs.combat_stats.get_fire_resist({})", uuid));
		auto frost_resist = evaluate_lua_double(std::format("rn.entity.prefabs.combat_stats.get_frost_resist({})", uuid));
		ImGui::Text("Physical Power: %.2f", physical_power);
		ImGui::TextColored(ImVec4{fire_colour[0], fire_colour[1], fire_colour[2], 1.0}, "Fire Power: %.2f", fire_power);
		ImGui::TextColored(ImVec4{frost_colour[0], frost_colour[1], frost_colour[2], 1.0}, "Frost Power: %.2f", frost_power);
		ImGui::Spacing();
		ImGui::Text("Physical Resist: %g%%", physical_resist * 100.0f);
		ImGui::TextColored(ImVec4{fire_colour[0], fire_colour[1], fire_colour[2], 1.0}, "Fire Resist: %g%%", fire_resist * 100.0f);
		ImGui::TextColored(ImVec4{frost_colour[0], frost_colour[1], frost_colour[2], 1.0}, "Frost Resist: %g%%", frost_resist * 100.0f);

		ImGui::Separator();
		ImGui::Text("Spell-casta-mabob");
		static std::string spell_name = "";
		ImGui::InputText("Spell Name", &spell_name);
		bool is_valid_spell_name = evaluate_lua_bool(std::format("rn.spell.spells.{} ~= nil", spell_name));
		if(is_valid_spell_name)
		{
			if(ImGui::Button("Cast"))
			{
				std::string code = std::format("rn.spell.cast({}, \"{}\")", uuid, spell_name);
				tz::lua::get_state().execute(code.c_str());
			}
		}
		else
		{
			ImGui::Text("\"%s\" is not a valid spell name.", spell_name.c_str());
		}
	}
	
	void scene::dbgui_combat_analyst()
	{
		ImGui::Text("It's analysin' time");
		static entity_uuid tracked_entity = null_uuid;
		if(ImGui::InputText("Entity UUID", &this->debug_dbgui_tracked_entity_input_string))
		{
			if(this->debug_dbgui_tracked_entity_input_string == "null")
			{
				tracked_entity = null_uuid;
			}
			else
			{
				// atoi returns 0 if conversion fails. so just accept the value directly.
				tracked_entity = std::atoi(this->debug_dbgui_tracked_entity_input_string.c_str());
			}
		}

		if(tracked_entity != null_uuid && this->contains_entity(tracked_entity))
		{
			dbgui_combat_ent(*this, tracked_entity);
		}
		else
		{
			ImGui::Text("No Entity Tracked");
		}
	}
}