#include "gamelib/gameplay/actor.hpp"
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
		this->transform.dbgui();
		this->entity.dbgui();
	}

	void actor::mount(quad_renderer::quad_data& quad) const
	{
		quad.pos = this->transform.get_position();
		quad.scale = this->transform.get_scale();
		quad.rotation = this->transform.get_rotation();
		// TODO: Texture support.
		quad.texid = 0u;
	}
}
