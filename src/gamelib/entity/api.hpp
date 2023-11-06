#ifndef RN_GAMELIB_ENTITY_API_HPP
#define RN_GAMELIB_ENTITY_API_HPP
#include "gamelib/renderer/scene_renderer.hpp"
#include "gamelib/logic/stats.hpp"
#include "tz/core/data/enum_field.hpp"

namespace game::entity
{
	enum class faction
	{
		// friendly with everyone
		pure_friend,
		// hostile with everyone
		pure_enemy,
		// neutral with everyone
		pure_neutral,
		// friendly with other player_ally, hostile with player_enemy, otherwise neutral
		player_ally,
		// friendly with other player_enemy, hostile with player_ally, otherwise neutral
		player_enemy,
		_count
	};
	constexpr std::array<const char*, (int)faction::_count> faction_names =
	{
		"pure friend",
		"pure enemy",
		"pure neutral",
		"player ally",
		"player enemy"
	};
	enum class relationship
	{
		friendly,
		neutral,
		hostile
	};
	enum class flag
	{
		no_collide,
		immoveable_collide
	};
	using flags_t = tz::enum_field<flag>;
	struct entity
	{
		static std::size_t uid_global_counter;
		std::size_t type = std::numeric_limits<std::size_t>::max();
		std::size_t uid = uid_global_counter++;
		faction allegience = faction::pure_neutral;
		std::string name = "Untitled Entity";
		game::logic::stats base_stats = {};
		std::uint64_t current_health = 0u;
		std::uint8_t level = 1u;
		std::unordered_map<std::string, game::logic::buff> buffs = {};
		game::logic::buff equipment_buff = {};
		game::render::scene_element elem = {};
		flags_t flags = {};

		game::logic::stats get_stats() const;
		relationship get_relationship(const entity& rhs) const;
		relationship get_relationship(faction allegience) const;
		void dbgui();
		void update(float delta_seconds);
		void level_up();
		void set_level(std::uint8_t new_level);

		static entity null()
		{
			return
			{
				.type = std::numeric_limits<std::size_t>::max(),
				.name = "Null Entity"		
			};
		}

		static std::string get_type_name(std::size_t type);
		static std::size_t get_type_count();
	};

	class scene;

	struct rn_impl_entity
	{
		scene* scene = nullptr;
		tz::hanval entity_hanval = static_cast<tz::hanval>(tz::handle<int>{tz::nullhand});

		entity& get();

		int is_valid(tz::lua::state& state);
		int get_type(tz::lua::state& state);
		int uid(tz::lua::state& state);
		int get_name(tz::lua::state& state);
		int set_name(tz::lua::state& state);
		int get_faction(tz::lua::state& state);
		int set_faction(tz::lua::state& state);
		int get_relationship(tz::lua::state& state);
		int get_base_stats(tz::lua::state& state);
		int set_base_stats(tz::lua::state& state);
		int get_stats(tz::lua::state& state);
		int get_health(tz::lua::state& state);
		int set_health(tz::lua::state& state);
		int get_level(tz::lua::state& state);
		int set_level(tz::lua::state& state);
		int level_up(tz::lua::state& state);
		int is_dead(tz::lua::state& state);
		int impl_set_equipment_buff(tz::lua::state& state);
		int apply_buff(tz::lua::state& state);
		int get_model(tz::lua::state& state);
		int set_model(tz::lua::state& state);
		int get_element(tz::lua::state& state);

		int is_collideable(tz::lua::state& state);
		int set_collideable(tz::lua::state& state);
		int is_immoveable(tz::lua::state& state);
		int set_immoveable(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(rn_impl_entity)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(rn_impl_entity, is_valid)
			LUA_METHOD(rn_impl_entity, get_type)
			LUA_METHOD(rn_impl_entity, uid)
			LUA_METHOD(rn_impl_entity, get_name)
			LUA_METHOD(rn_impl_entity, set_name)
			LUA_METHOD(rn_impl_entity, get_faction)
			LUA_METHOD(rn_impl_entity, set_faction)
			LUA_METHOD(rn_impl_entity, get_relationship)
			LUA_METHOD(rn_impl_entity, get_base_stats)
			LUA_METHOD(rn_impl_entity, set_base_stats)
			LUA_METHOD(rn_impl_entity, get_stats)
			LUA_METHOD(rn_impl_entity, get_health)
			LUA_METHOD(rn_impl_entity, set_health)
			LUA_METHOD(rn_impl_entity, get_level)
			LUA_METHOD(rn_impl_entity, set_level)
			LUA_METHOD(rn_impl_entity, level_up)
			LUA_METHOD(rn_impl_entity, is_dead)
			LUA_METHOD(rn_impl_entity, impl_set_equipment_buff)
			LUA_METHOD(rn_impl_entity, apply_buff)
			LUA_METHOD(rn_impl_entity, get_model)
			LUA_METHOD(rn_impl_entity, set_model)
			LUA_METHOD(rn_impl_entity, get_element)

			LUA_METHOD(rn_impl_entity, is_collideable)
			LUA_METHOD(rn_impl_entity, set_collideable)
			LUA_METHOD(rn_impl_entity, is_immoveable)
			LUA_METHOD(rn_impl_entity, set_immoveable)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END
}

#endif // RN_GAMELIB_ENTITY_API_HPP