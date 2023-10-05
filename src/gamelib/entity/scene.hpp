#ifndef RN_GAMELIB_ENTITY_SCENE_HPP
#define RN_GAMELIB_ENTITY_SCENE_HPP
#include "gamelib/renderer/scene_renderer.hpp"

namespace game::entity
{
	class scene
	{
	public:
		scene() = default;
		render::scene_renderer& get_renderer();
		void lua_initialise(tz::lua::state& state);
	private:
		render::scene_renderer renderer;
	};
}

#endif // RN_GAMELIB_ENTITY_SCENE_HPP