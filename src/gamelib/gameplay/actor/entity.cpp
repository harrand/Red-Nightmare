#include "gamelib/gameplay/actor/entity.hpp"
#include "gamelib/gameplay/actor/system.hpp"
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
			res << component_ptr->mount(quads.subspan(res.count));
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
		if(this->target == nullid)
		{
			ImGui::Text("No target");
		}
		else
		{
			ImGui::Text("Target: %zu", this->target);
		}
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
			if(ImGui::Button("Mitosis"))
			{
				this->actions.add_component<action_id::spawn>
				({
					.type = this->type,
		 			.actions = this->actions
				});
			}
			if(ImGui::Button("Cast Teleport"))
			{
				this->actions.add_component<action_id::cast>
				({
					.spell = spell_id::teleport
				});
			}
		}
		ImGui::Unindent();
	}

	void actor::update(float dt)
	{
		TZ_PROFZONE("actor - update", 0xffee0077);
		this->entity.update(dt, *this);
	}

	mount_result actor::mount(std::span<quad_renderer::quad_data> quads, const actor_system& sys) const
	{
		TZ_PROFZONE("actor - mount", 0xffee0077);
		mount_result res = this->entity.mount(quads);
		transform_t t = sys.get_global_transform(this->uuid);
		for(std::size_t i = 0; i < res.count; i++)
		{
			quads[i].pos += t.position;
			quads[i].scale[0] *= t.scale[0];
			quads[i].scale[1] *= t.scale[1];
			quads[i].rotation = t.rotation;
			quads[i].layer += this->layer;
		}
		return res;
	}

	bool actor::dead() const
	{
		return this->entity.has_component<actor_component_id::damageable>() && this->entity.get_component<actor_component_id::damageable>()->data().dead();
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
				case actor_type::heal_effect:
				{
					#include "gamelib/gameplay/actor/types/heal_effect.actor"
				}
				break;
				case actor_type::impact_remnant:
				{
					#include "gamelib/gameplay/actor/types/impact_remnant.actor"
				}
				break;
				case actor_type::oak_tree:
				{
					#include "gamelib/gameplay/actor/types/oak_tree.actor"
				}
				break;
				case actor_type::doctor_michael_morbius:
				{
					#include "gamelib/gameplay/actor/types/doctor_michael_morbius.actor"
				}
				break;
				case actor_type::fireball:
				{
					#include "gamelib/gameplay/actor/types/fireball.actor"
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
				case actor_type::unknown:
					return {};
				break;
			}
			tz::error("Attempted to spawn actor of an unknown type. Add support for the actor.");	
			return {};
		}(type);
		a.type = type;
		return a;
	}
}
