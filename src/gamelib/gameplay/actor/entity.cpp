#include "gamelib/gameplay/actor/entity.hpp"
#include "tz/dbgui/dbgui.hpp"
#include <random> // randomius.actor

namespace rnlib
{
	std::size_t actor::uuid_count = 0;

	mount_result actor_entity::mount(std::span<quad_renderer::quad_data> quads) const
	{
		mount_result res;
		for(auto& component_ptr : this->components)
		{
			res << component_ptr->mount(quads);
		}
		return res;
	}

	void actor_entity::update(float dt, actor& actor)
	{
		for(auto& component_ptr : this->components)
		{
			component_ptr->update(dt, actor);
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
		ImGui::Unindent();
	}

	void actor::update(float dt)
	{
		this->entity.update(dt, *this);
	}

	mount_result actor::mount(std::span<quad_renderer::quad_data> quads) const
	{
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

	// prefab implementations.
	namespace prefab
	{
		#include "gamelib/gameplay/actor/prefabs/human.chest.prefab"
		#include "gamelib/gameplay/actor/prefabs/human.eyes.prefab"
		#include "gamelib/gameplay/actor/prefabs/human.feet.prefab"
		#include "gamelib/gameplay/actor/prefabs/human.hair.prefab"
		#include "gamelib/gameplay/actor/prefabs/human.helm.prefab"
		#include "gamelib/gameplay/actor/prefabs/human.actor.prefab"
	}

	actor create_actor(actor_type type)
	{
		actor a = [](actor_type t)->actor
		{
			switch(t)
			{
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
