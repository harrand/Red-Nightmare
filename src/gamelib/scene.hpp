#ifndef REDNIGHTMARE_GAMELIB_SCENE_HPP
#define REDNIGHTMARE_GAMELIB_SCENE_HPP
#include "gamelib/entity.hpp"
#include "gamelib/render/scene_renderer.hpp"
#include "gamelib/physics/grid_hierarchy.hpp"
#include "tz/core/data/free_list.hpp"
#include "tz/core/job/job.hpp"
#include <unordered_map>

namespace game
{
	struct scene_entity_data
	{
		game::entity ent;
		game::render::scene_renderer::entry ren;

		static scene_entity_data null()
		{
			return {.ent = game::entity::null(), .ren = {.obj = tz::nullhand}};
		}

		bool is_null() const
		{
			return this->ent.is_null();
		}
	};

	class scene
	{
	public:
		scene();
		using entity_handle = tz::free_list<scene_entity_data>::handle;

		entity_handle add_entity(entity_uuid uuid);
		entity_handle add_entity_from_prefab(entity_uuid uuid, const std::string& prefab_name);
		entity_handle add_entity_from_existing(entity_uuid uuid, entity_uuid existing);
		void remove_entity(entity_handle e);
		void remove_entity(entity_uuid uuid);
		void clear();
		void update(float delta_seconds);
		void fixed_update(float delta_seconds, std::uint64_t unprocessed);
		void block();

		bool contains_entity(entity_uuid uuid) const;
		bool contains_entity(entity_handle e) const;

		std::size_t entity_count() const;
		const game::entity& get_entity(entity_handle e) const;
		game::entity& get_entity(entity_handle e);
		const game::entity& get_entity(entity_uuid uuid) const;
		game::entity& get_entity(entity_uuid uuid);

		const game::render::scene_renderer::entry& get_entity_render_component(entity_uuid uuid) const;
		const game::render::scene_renderer::entry& get_entity_render_component(entity_handle e) const;

		const game::render::scene_renderer& get_renderer() const;
		game::render::scene_renderer& get_renderer();

		physics::grid_hierarchy& get_grid();
		const physics::grid_hierarchy& get_grid() const;

		void notify_new_entity(entity_uuid uuid);
		void notify_entity_change(entity_uuid uuid);

		tz::vec2 get_mouse_position_world_space() const;

		physics::intersection_data_view get_intersections();
		void dbgui();
		void dbgui_combat_analyst();
	private:
		bool wants_collision_detection(entity_uuid uuid) const;
		physics::boundary_t bound_entity(entity_uuid uuid) const;
		void initialise_renderer_component(entity_uuid uuid);
		tz::vec2 calc_mouse_position_world_space() const;
		bool single_collision_response(entity_uuid entity_a, entity_uuid entity_b, physics::boundary_t::manifold manifold);
		// free list gives handle stability, which we want.
		// hashmap gives fast lookup for those who want to index by uuid (which everyone will want to do)
		tz::free_list<scene_entity_data> entities = {};
		std::unordered_map<entity_uuid, entity_handle> uuid_entity_map = {};
		std::vector<tz::job_handle> entity_update_jobs = {};
		game::render::scene_renderer renderer;
		tz::vec2 mouse_pos_ws = {};
		std::string debug_dbgui_tracked_entity_input_string;
		physics::grid_hierarchy grid;
	public:
		decltype(entities)::iterator begin();
		decltype(entities)::const_iterator begin() const ;
		decltype(entities)::iterator end();
		decltype(entities)::const_iterator end() const;
	};
}

#endif // REDNIGHTMARE_GAMELIB_SCENE_HPP
