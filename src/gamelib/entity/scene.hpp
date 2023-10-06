#ifndef RN_GAMELIB_ENTITY_SCENE_HPP
#define RN_GAMELIB_ENTITY_SCENE_HPP
#include "gamelib/renderer/scene_renderer.hpp"
#include "gamelib/entity/api.hpp"
#include <deque>

namespace game::entity
{
	class scene
	{
	public:
		scene() = default;
		using entity_handle = tz::handle<entity>;

		entity_handle add(std::size_t type);
		void remove(entity_handle e);
		const entity& get(entity_handle e) const;
		entity& get(entity_handle e);

		render::scene_renderer& get_renderer();
		void lua_initialise(tz::lua::state& state);
	private:
		void initialise_entity(tz::hanval entity_hanval, std::size_t type);
		std::vector<entity> entities = {};
		std::deque<entity_handle> free_list = {};
		render::scene_renderer renderer;
	};

	struct rn_impl_scene
	{
		scene* sc;
		int add(tz::lua::state& state);
		int remove(tz::lua::state& state);
		int get(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(rn_impl_scene)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(rn_impl_scene, add)
			LUA_METHOD(rn_impl_scene, remove)
			LUA_METHOD(rn_impl_scene, get)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END
}

#endif // RN_GAMELIB_ENTITY_SCENE_HPP