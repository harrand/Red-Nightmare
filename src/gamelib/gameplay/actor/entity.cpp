#include "gamelib/gameplay/actor/entity.hpp"
#include "tz/dbgui/dbgui.hpp"
#include <random> // randomius.actor

namespace rnlib
{
	std::size_t actor::uuid_count = 0;

	mount_result actor_entity::mount(std::span<quad_renderer::quad_data> quads) const
	{
		TZ_PROFZONE("actor_entity - mount", 0xffee0077);
		mount_result res;
		for(auto& component_ptr : this->components)
		{
			res << component_ptr->mount(quads);
		}
		return res;
	}

	void actor_entity::update(float dt, actor& actor)
	{
		TZ_PROFZONE("actor_entity - update", 0xffee0077);
		std::vector<iactor_component*> components;
		components.reserve(this->components.size());
		// component update may well add new components, so keep a copy of them for now.
		for(auto& component_ptr : this->components)
		{
			components.push_back(component_ptr.get());
		}
		for(iactor_component* cmp : components)
		{
			cmp->update(dt, actor);
		}
	}

	void actor_entity::dbgui()
	{
		if(ImGui::CollapsingHeader("Components"))
		{
			ImGui::Indent();
			for(auto& component_ptr : this->components)
			{
				component_ptr->dbgui();
				ImGui::Separator();
			}
			ImGui::Unindent();
		}
	}

	void actor::dbgui()
	{
		ImGui::Text("%s (%zu)", this->name, this->uuid);
		ImGui::Indent();
		this->transform.dbgui();
		this->entity.dbgui();
		if(ImGui::CollapsingHeader("Actions"))
		{
			this->actions.dbgui();
			if(ImGui::Button("Random Teleport"))
			{
				this->actions.set_component<action_id::random_teleport>();			
			}
			if(ImGui::Button("Debug Move to Origin"))
			{
				this->actions.set_component<action_id::move_to>({.location = tz::vec2::zero()});
			}
			if(ImGui::Button("Despawn"))
			{
				this->actions.add_component<action_id::despawn>();
			}
		}
		ImGui::Unindent();
	}

	void actor::update(float dt)
	{
		TZ_PROFZONE("actor - update", 0xffee0077);
		this->entity.update(dt, *this);
	}

	mount_result actor::mount(std::span<quad_renderer::quad_data> quads) const
	{
		TZ_PROFZONE("actor - mount", 0xffee0077);
		mount_result res = this->entity.mount(quads);
		for(std::size_t i = 0; i < res.count; i++)
		{
			quads[i].pos += this->transform.get_position();
			quads[i].scale = this->transform.get_scale();
			quads[i].rotation = this->transform.get_rotation();
			quads[i].layer += this->layer;
		}
		return res;
	}

	actor create_actor(actor_type type)
	{
		TZ_PROFZONE("actor - create_actor", 0xffee0077);
		actor a = [](actor_type t)->actor
		{
			switch(t)
			{
				case actor_type::dev_barrier:
				{
					#include "gamelib/gameplay/actor/types/dev_barrier.actor"
				}
				break;
				case actor_type::doctor_michael_morbius:
				{
					#include "gamelib/gameplay/actor/types/doctor_michael_morbius.actor"
				}
				break;
				case actor_type::player_melistra:
				{
					#include "gamelib/gameplay/actor/types/player_melistra.actor"
				}
				break;
				case actor_type::player_benedict:
				{
					#include "gamelib/gameplay/actor/types/player_benedict.actor"
				}
				break;
				case actor_type::randomius:
				{
					#include "gamelib/gameplay/actor/types/randomius.actor"
				}
				break;
				case actor_type::the_black_knight:
				{
					#include "gamelib/gameplay/actor/types/the_black_knight.actor"
				}
				break;
			}
			return {};
		}(type);
		a.type = type;
		return a;
	}
}
