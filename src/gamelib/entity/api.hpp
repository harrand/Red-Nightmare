#ifndef RN_GAMELIB_ENTITY_API_HPP
#define RN_GAMELIB_ENTITY_API_HPP
#include "gamelib/renderer/scene_renderer.hpp"

namespace game::entity
{
	struct entity
	{
		std::size_t type = 0;
		std::string name = "Untitled Entity";
		game::render::scene_element elem = {};

		static entity null()
		{
			return
			{
				.type = std::numeric_limits<std::size_t>::max(),
				.name = "Null Entity"		
			};
		}
	};

	class scene;

	struct rn_impl_entity
	{
		scene* scene = nullptr;
		tz::hanval entity_hanval = static_cast<tz::hanval>(tz::handle<int>{tz::nullhand});

		entity& get();

		int get_type(tz::lua::state& state);
		int get_name(tz::lua::state& state);
		int set_name(tz::lua::state& state);
		int get_model(tz::lua::state& state);
		int set_model(tz::lua::state& state);
		int get_element(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(rn_impl_entity)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(rn_impl_entity, get_type)
			LUA_METHOD(rn_impl_entity, get_name)
			LUA_METHOD(rn_impl_entity, set_name)
			LUA_METHOD(rn_impl_entity, get_model)
			LUA_METHOD(rn_impl_entity, set_model)
			LUA_METHOD(rn_impl_entity, get_element)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END
}

#endif // RN_GAMELIB_ENTITY_API_HPP