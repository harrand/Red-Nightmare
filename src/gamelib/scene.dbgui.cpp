#include "gamelib/scene.hpp"

namespace game
{
	void dbgui_ent(const scene_entity_data& edata)
	{
		std::string tree_title = std::format("{}: {}", edata.ent.uuid, edata.ent.name);
		if(ImGui::TreeNode(tree_title.c_str()))
		{
			ImGui::Text("Model: %s", edata.ren.obj == tz::nullhand ? "None" : edata.ren.model_name.c_str());
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
				dbgui_ent(edata);
			}
		}
	}
}