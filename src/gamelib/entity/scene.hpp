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
		using light_handle = tz::handle<render::scene_renderer::light_data>;

		entity_handle add(std::size_t type);
		void remove(entity_handle e);
		void clear();
		void clear_except_players();
		const entity& get(entity_handle e) const;
		entity& get(entity_handle e);
		bool is_valid(entity_handle e) const;
		std::size_t size() const;

		light_handle add_light(render::scene_renderer::point_light_data d = {});
		void remove_light(light_handle l);
		const render::scene_renderer::point_light_data& get_light(light_handle l) const;
		render::scene_renderer::point_light_data& get_light(light_handle l);

		void update(float delta_seconds);
		void block();
		void dbgui();
		void dbgui_game_bar();

		const render::scene_renderer& get_renderer() const;
		render::scene_renderer& get_renderer();

		unsigned int get_player_credits() const;
		void set_player_credits(unsigned int credits);

		tz::vec2 get_mouse_position_ws() const;

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
		void collision_response(float delta_seconds);
		void resolve_collision(entity_handle ah, entity_handle bh, float delta_seconds);
		void advance_camera(float delta_seconds);
		bool is_valid(tz::hanval entity_hanval) const;
		void dbgui_impl();
		std::vector<entity> entities = {};
		std::deque<entity_handle> free_list = {};
		std::deque<light_handle> light_free_list = {};
		std::size_t light_cursor = 0;
		scene_quadtree quadtree{game::physics::aabb{tz::vec2{-1.0f, -1.0f} * scene_quadtree_initial_size, tz::vec2{1.0f, 1.0f} * scene_quadtree_initial_size}};
		scene_quadtree::intersection_state_t intersection_state = {};
		// uid maps to a set of entity handles. i know that seems inconsistent but its cheapest for the lua to use.
		mutable std::unordered_map<std::size_t, std::unordered_set<tz::hanval>> collision_data = {};
		render::scene_renderer renderer;
		unsigned int player_credits = 0;
	};

	struct rn_impl_scene
	{
		scene* sc;
		int add(tz::lua::state& state);
		int add_light(tz::lua::state& state);
		int remove(tz::lua::state& state);
		int remove_uid(tz::lua::state& state);
		int remove_light(tz::lua::state& state);
		int get_light(tz::lua::state& state);
		int clear(tz::lua::state& state);
		int clear_except_players(tz::lua::state& state);
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
			LUA_METHOD(rn_impl_scene, add_light)
			LUA_METHOD(rn_impl_scene, remove)
			LUA_METHOD(rn_impl_scene, remove_uid)
			LUA_METHOD(rn_impl_scene, remove_light)
			LUA_METHOD(rn_impl_scene, get_light)
			LUA_METHOD(rn_impl_scene, clear)
			LUA_METHOD(rn_impl_scene, clear_except_players)
			LUA_METHOD(rn_impl_scene, get_collision_count)
			LUA_METHOD(rn_impl_scene, get_collision)
			LUA_METHOD(rn_impl_scene, get)
			LUA_METHOD(rn_impl_scene, get_uid)
			LUA_METHOD(rn_impl_scene, get_renderer)
			LUA_METHOD(rn_impl_scene, size)
			LUA_METHOD(rn_impl_scene, get_mouse_position_ws)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END

	struct rn_impl_light
	{
		scene* sc;
		scene::light_handle l = tz::nullhand;

		int get_position(tz::lua::state& state);
		int set_position(tz::lua::state& state);
		int get_colour(tz::lua::state& state);
		int set_colour(tz::lua::state& state);
		int get_power(tz::lua::state& state);
		int set_power(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(rn_impl_light)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(rn_impl_light, get_position)
			LUA_METHOD(rn_impl_light, set_position)
			LUA_METHOD(rn_impl_light, get_colour)
			LUA_METHOD(rn_impl_light, set_colour)
			LUA_METHOD(rn_impl_light, get_power)
			LUA_METHOD(rn_impl_light, set_power)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END
}

#endif // RN_GAMELIB_ENTITY_SCENE_HPP