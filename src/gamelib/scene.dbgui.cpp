#include "gamelib/scene.hpp"
#include "gamelib/render/scene_renderer.hpp"

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
		if(ImGui::TreeNode(tree_title.c_str()))
		{
			dbgui_ent_ren(edata.ren, renderer);
			std::string subtree_title = std::format("Internals ({})", edata.ent.internal_variables.size());
			if(edata.ent.internal_variables.size() && ImGui::TreeNode(subtree_title.c_str()))
			{
				for(const auto& [name, val] : edata.ent.internal_variables)
				{
					std::string value_str;	
					std::visit([&value_str](auto&& arg)
					{
						using T = std::decay_t<decltype(arg)>;
						if constexpr(std::is_same_v<T, bool>)
						{
							value_str = arg ? "true" : "false";
						}
						else if constexpr(std::is_same_v<T, std::string>)
						{
							value_str = std::format("\"{}\"", arg);
						}
						else if constexpr(std::is_same_v<T, tz::lua::nil>)
						{
							(void)arg;
							value_str = "nil";
						}
						else
						{
							value_str = std::to_string(arg);
						}
					}, val);
					ImGui::Text("%s | %s", name.c_str(), value_str.c_str());
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
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