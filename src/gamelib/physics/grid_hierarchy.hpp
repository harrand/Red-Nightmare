#ifndef REDNIGHTMARE_GAMELIB_PHYSICS_GRID_HIERARCHY_HPP
#define REDNIGHTMARE_GAMELIB_PHYSICS_GRID_HIERARCHY_HPP
#include "gamelib/physics/aabb.hpp"
#include "gamelib/entity.hpp"
#include "tz/core/data/vector.hpp"

namespace game::physics
{
	using boundary_t = aabb;
	using intersection_data = std::vector<std::tuple<entity_uuid, entity_uuid, boundary_t::manifold>>;
	using intersection_data_view = std::span<const intersection_data::value_type>;
	// grid hierarchy. cell size is uniform.
	class grid_hierarchy
	{
	public:
		using cell_coord = tz::vec2us;
		constexpr static cell_coord nullcoord = cell_coord{std::numeric_limits<unsigned short>::max(), std::numeric_limits<unsigned short>::max()};
		enum class oob_policy
		{
			assert_in_bounds,
			discard
		};

		grid_hierarchy(tz::vec2 world_centre, tz::vec2 world_bounds, cell_coord cell_dimensions);
		// invoke whenever world boundaries change.
		void set_dimensions(tz::vec2 world_centre, tz::vec2 world_bounds, cell_coord cell_dimensions);
		// add a new entity for collision.
		void add_entity(entity_uuid uuid, boundary_t boundary, oob_policy oob = oob_policy::assert_in_bounds);
		// remove a bounded entity from the simulation.
		void remove_entity(entity_uuid uuid);
		// notify the simulation that the boundary for an existing entity has changed (e.g its moved, or its dimensions have changed)
		void notify_change(entity_uuid uuid, boundary_t boundary, oob_policy oob = oob_policy::assert_in_bounds);

		const intersection_data& get_intersections();
		// remove all entities in the simulation. world boundaries stay the same.
		void clear();
	private:
		struct cell_t;

		cell_coord notify_change_without_cache_changes(entity_uuid uuid, boundary_t boundary, oob_policy oob);
		void create_cells();
		const cell_t& get_cell(cell_coord coord) const;
		cell_t& get_cell(cell_coord coord);

		struct cell_t
		{
			cell_coord coord;
			std::vector<entity_uuid> contained_entities = {};
		};

		tz::vec2 centre = {0.0f, 0.0f};
		tz::vec2 bounds = {0.0f, 0.0f};
		cell_coord cell_dimensions = static_cast<cell_coord>(tz::vec2ui{1u, 1u});
		std::vector<cell_t> cells = {cell_t{}};
		std::unordered_map<entity_uuid, boundary_t> bounded_entities = {};
		std::optional<intersection_data> cached_intersections = std::nullopt;
	};
}

#endif // REDNIGHTMARE_GAMELIB_PHYSICS_GRID_HIERARCHY_HPP