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
				ImGui::SetNextItemOpen(mod.name == "basegame");
				if(ImGui::TreeNode(mod.name.c_str()))
				{
					ImGui::Text("Description: %s", mod.description.c_str());
					if(owned_prefabs.size() && ImGui::TreeNode("Prefabs"))
					{
						static bool hide_prefabs_without_pre_instantiate = false;
						static bool hide_prefabs_without_on_collision = false;
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
					ImGui::TreePop();
				}
			}
		}
	}
}