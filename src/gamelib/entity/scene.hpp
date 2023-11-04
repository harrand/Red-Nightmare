#ifndef RN_GAMELIB_ENTITY_SCENE_HPP
#define RN_GAMELIB_ENTITY_SCENE_HPP
#include "gamelib/renderer/scene_renderer.hpp"
#include "gamelib/entity/api.hpp"
#include "gamelib/physics/quadtree.hpp"
#include <deque>
#include <unordered_map>
#include <unordered_set>

namespace game::entity
{
	struct scene_quadtree_node
	{
		scene* sc = nullptr;
		tz::hanval entity_hanval = static_cast<tz::hanval>(tz::handle<int>{tz::nullhand});

		game::physics::aabb get_aabb() const;
		inline bool operator==(const scene_quadtree_node& rhs) const{return this->entity_hanval != rhs.entity_hanval;}
	};

	using scene_quadtree = game::physics::quadtree<scene_quadtree_node>;
	constexpr float scene_quadtree_initial_size = 500.0f;

	class scene
	{
	public:
		scene() = default;
		using entity_handle = tz::handle<entity>;

		entity_handle add(std::size_t type);
		void remove(entity_handle e);
		const entity& get(entity_handle e) const;
		entity& get(entity_handle e);
		std::size_t size() const;

		void update(float delta_seconds);
		void block();
		void dbgui();
		void dbgui_game_bar();

		render::scene_renderer& get_renderer();
		std::size_t debug_get_intersection_count() const;
		std::size_t get_collision_count(entity_handle e) const;
		std::size_t get_collision_count(std::size_t uid) const;
		entity_handle get_collision_id(entity_handle e, std::size_t id) const;
		entity_handle get_collision_id(std::size_t uid, std::size_t id) const;
		void lua_initialise(tz::lua::state& state);
	private:
		void initialise_entity(tz::hanval entity_hanval, std::size_t type);
		void deinitialise_entity(tz::hanval entity_hanval, std::size_t uid);
		entity_handle try_find_player() const;
		void rebuild_quadtree();
		void advance_camera(float delta_seconds);
		bool is_valid(tz::hanval entity_hanval) const;
		void dbgui_impl();
		std::vector<entity> entities = {};
		std::deque<entity_handle> free_list = {};
		scene_quadtree quadtree{game::physics::aabb{tz::vec2{-1.0f, -1.0f} * scene_quadtree_initial_size, tz::vec2{1.0f, 1.0f} * scene_quadtree_initial_size}};
		scene_quadtree::intersection_state_t intersection_state = {};
		// uid maps to a set of entity handles. i know that seems inconsistent but its cheapest for the lua to use.
		mutable std::unordered_map<std::size_t, std::unordered_set<tz::hanval>> collision_data = {};
		render::scene_renderer renderer;
	};

	struct rn_impl_scene
	{
		scene* sc;
		int add(tz::lua::state& state);
		int remove(tz::lua::state& state);
		int remove_uid(tz::lua::state& state);
		int get_collision_count(tz::lua::state& state);
		int get_collision(tz::lua::state& state);
		int get(tz::lua::state& state);
		int get_uid(tz::lua::state& state);
		int get_renderer(tz::lua::state& state);
		int size(tz::lua::state& state);
		int get_mouse_position_ws(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(rn_impl_scene)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(rn_impl_scene, add)
			LUA_METHOD(rn_impl_scene, remove)
			LUA_METHOD(rn_impl_scene, remove_uid)
			LUA_METHOD(rn_impl_scene, get_collision_count)
			LUA_METHOD(rn_impl_scene, get_collision)
			LUA_METHOD(rn_impl_scene, get)
			LUA_METHOD(rn_impl_scene, get_uid)
			LUA_METHOD(rn_impl_scene, get_renderer)
			LUA_METHOD(rn_impl_scene, size)
			LUA_METHOD(rn_impl_scene, get_mouse_position_ws)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END
}

#endif // RN_GAMELIB_ENTITY_SCENE_HPP