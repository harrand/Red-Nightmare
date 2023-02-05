#include "gamelib/gameplay/actor/entity.hpp"
#include "tz/dbgui/dbgui.hpp"

namespace rnlib
{
	std::size_t actor::uuid_count = 0;

	void actor_entity::dbgui()
	{

	}

	void actor::dbgui()
	{
		ImGui::Text("%s (%zu)", this->name, this->uuid);
		ImGui::Indent();
		this->transform.dbgui();
		this->entity.dbgui();
		ImGui::Unindent();
	}

	template<actor_component_id ID>
	mount_result mount_impl(const actor_entity& ent, std::span<quad_renderer::quad_data> quads)
	{
		if(ent.has_component<ID>())
		{
			return actor_component_mount<ID>(*ent.get_component<ID>(), quads);
		}
		return {};
	}

	mount_result actor::mount(std::span<quad_renderer::quad_data> quads) const
	{
		mount_result res;
		res << mount_impl<actor_component_id::sprite>(this->entity, quads);
		for(std::size_t i = 0; i < res.count; i++)
		{
			quads[i].pos += this->transform.get_position();
			quads[i].scale = this->transform.get_scale();
			quads[i].rotation = this->transform.get_rotation();
		}
		return res;
	}

	actor create_actor(actor_type type)
	{
		actor a = [](actor_type t)->actor
		{
			switch(t)
			{
				case actor_type::player_akhara:
				#include "gamelib/gameplay/actor/types/player_akhara.actor"
			}
			return {};
		}(type);
		a.type = type;
		return a;
	}
}
