#include "gamelib/gameplay/actor/system.hpp"
#include "tz/dbgui/dbgui.hpp"

namespace rnlib
{
	const actor* actor_system::find(std::size_t uuid) const
	{
		auto iter = std::find_if(this->entities.begin(), this->entities.end(),
		[uuid](const actor& entity)
		{
			return entity.uuid == uuid;
		});
		if(iter == this->entities.end())
		{
			return nullptr;
		}
		return &*iter;
	}

	actor* actor_system::find(std::size_t uuid)
	{
		auto iter = std::find_if(this->entities.begin(), this->entities.end(),
		[uuid](const actor& entity)
		{
			return entity.uuid == uuid;
		});
		if(iter == this->entities.end())
		{
			return nullptr;
		}
		return &*iter;
	}

	mount_result actor_system::mount(std::span<quad_renderer::quad_data> quads)
	{
		mount_result res;
		for(const actor& entity : this->entities)
		{
			res << entity.mount(quads.subspan(res.count));
		}
		return res;
	}

	void actor_system::update(float dt)
	{
		for(actor& entity : this->entities)
		{
			entity.update(dt);
		}
	}

	void actor_system::dbgui()
	{
		if(ImGui::BeginTabBar("Actor"))
		{
			if(ImGui::BeginTabItem("Actors List"))
			{
				static int entity_id = 0;
				if(this->entities.size())
				{
					ImGui::SliderInt("Entity ID", &entity_id, 0, this->entities.size() - 1);
					ImGui::Indent();
					this->entities[entity_id].dbgui();
					ImGui::Unindent();
				}
				else
				{
					ImGui::Text("No actors to display :(");
				}
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Debug Operations"))
			{
				static const char* combo_preview = "Actor types...";
				static std::optional<actor_type> maybe_type = std::nullopt;
				if(ImGui::BeginCombo("Create actor", combo_preview))
				{
					for(int i = 0 ; i < static_cast<int>(actor_type::_count); i++)
					{
						auto type = static_cast<actor_type>(i);
						if(type == actor_type::undefined)
						{
							continue;
						}
						bool is_selected = (maybe_type.has_value() && maybe_type.value() == type);
						const char* actor_name = create_actor(type).name;
						if(ImGui::Selectable(actor_name, is_selected))
						{
							maybe_type = type;
							combo_preview = actor_name;
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
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}
}
