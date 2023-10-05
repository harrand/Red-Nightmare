#include "gamelib/entity/scene.hpp"

namespace game::entity
{
	render::scene_renderer& scene::get_renderer()
	{
		return this->renderer;
	}

	void scene::lua_initialise(tz::lua::state& state)
	{
		this->get_renderer().lua_initialise(state);
	}
}