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
	void mount_impl(const actor_entity& ent, quad_renderer::quad_data& quad)
	{
		if(ent.has_component<ID>())
		{
			actor_component_mount<ID>(*ent.get_component<ID>(), quad);
		}
	}

	void actor::mount(quad_renderer::quad_data& quad) const
	{
		quad.pos = this->transform.get_position();
		quad.scale = this->transform.get_scale();
		quad.rotation = this->transform.get_rotation();

		mount_impl<actor_component_id::sprite>(this->entity, quad);
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
