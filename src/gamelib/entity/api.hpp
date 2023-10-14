#ifndef RN_GAMELIB_ENTITY_API_HPP
#define RN_GAMELIB_ENTITY_API_HPP
#include "gamelib/renderer/scene_renderer.hpp"
#include "gamelib/logic/stats.hpp"

namespace game::entity
{
	struct entity
	{
		static std::size_t uid_global_counter;
		std::size_t type = 0;
		std::size_t uid = uid_global_counter++;
		std::string name = "Untitled Entity";
		game::logic::stats base_stats = {};
		std::uint16_t current_health = 0u;
		std::unordered_map<std::string, game::logic::buff> buffs = {};
		game::render::scene_element elem = {};

		game::logic::stats get_stats() const;
		void update(float delta_seconds);

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
		int uid(tz::lua::state& state);
		int get_name(tz::lua::state& state);
		int set_name(tz::lua::state& state);
		int get_base_stats(tz::lua::state& state);
		int set_base_stats(tz::lua::state& state);
		int get_stats(tz::lua::state& state);
		int get_health(tz::lua::state& state);
		int set_health(tz::lua::state& state);
		int apply_buff(tz::lua::state& state);
		int get_model(tz::lua::state& state);
		int set_model(tz::lua::state& state);
		int get_element(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(rn_impl_entity)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(rn_impl_entity, get_type)
			LUA_METHOD(rn_impl_entity, uid)
			LUA_METHOD(rn_impl_entity, get_name)
			LUA_METHOD(rn_impl_entity, set_name)
			LUA_METHOD(rn_impl_entity, get_base_stats)
			LUA_METHOD(rn_impl_entity, set_base_stats)
			LUA_METHOD(rn_impl_entity, get_stats)
			LUA_METHOD(rn_impl_entity, get_health)
			LUA_METHOD(rn_impl_entity, set_health)
			LUA_METHOD(rn_impl_entity, apply_buff)
			LUA_METHOD(rn_impl_entity, get_model)
			LUA_METHOD(rn_impl_entity, set_model)
			LUA_METHOD(rn_impl_entity, get_element)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END
}

#endif // RN_GAMELIB_ENTITY_API_HPP