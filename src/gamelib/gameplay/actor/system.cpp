#include "gamelib/gameplay/actor/system.hpp"
#include "tz/dbgui/dbgui.hpp"

namespace rnlib
{
	std::size_t actor_system::mount(std::span<quad_renderer::quad_data> quads)
	{
		std::size_t count = 0;
		for(const actor& entity : this->entities)
		{
			count += entity.mount(quads.subspan(count));
		}
		return count;
	}

	void actor_system::dbgui()
	{
		if(ImGui::CollapsingHeader("Debug Operations"))
		{
			static std::optional<actor_type> maybe_type = std::nullopt;
			if(ImGui::BeginCombo("Create actor", "Actor types..."))
			{
				for(int i = 0 ; i < static_cast<int>(actor_type::_count); i++)
				{
					auto type = static_cast<actor_type>(i);
					if(type == actor_type::undefined)
					{
						continue;
					}
					bool is_selected = (maybe_type.has_value() && maybe_type.value() == type);
					if(ImGui::Selectable(create_actor(type).name, is_selected))
					{
						maybe_type = type;
					}
					if(is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if(maybe_type.has_value())
			{
				if(ImGui::Button("Create"))
				{
					this->entities.push_back(rnlib::create_actor(maybe_type.value()));
				}
			}
			if(this->entities.size())
			{
				if(ImGui::Button("Pop Back"))
				{
					this->entities.pop_back();
				}
				if(ImGui::Button("Clear"))
				{
					this->entities.clear();
				}
			}
		}
	}
}
