#include "gamelib/scene.hpp"
#include "gamelib/render/scene_renderer.hpp"
#include "gamelib/messaging/scene.hpp"
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
		if(ImGui::TreeNode(tree_title.c_str()))
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
		if(ImGui::CollapsingHeader("Entities"))
		{
			for(const scene_entity_data& edata : *this)
			{
				dbgui_ent(edata, this->get_renderer());
			}
		}
	}
}