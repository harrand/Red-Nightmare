#ifndef REDNIGHTMARE_GAMELIB_PHYSICS_GRID_HIERARCHY_HPP
#define REDNIGHTMARE_GAMELIB_PHYSICS_GRID_HIERARCHY_HPP
#include "gamelib/physics/aabb.hpp"
#include "gamelib/entity.hpp"
#include "tz/core/data/vector.hpp"

namespace game::physics
{
	using boundary_t = aabb;
	// grid hierarchy. cell size is uniform.
	class grid_hierarchy
	{
	public:
		enum class oob_policy
		{
			assert_in_bounds,
			discard
		};

		grid_hierarchy() = default;
		// invoke whenever world boundaries change.
		void set_dimensions(tz::vec2 world_centre, tz::vec2 world_bounds);
		// add a new entity for collision.
		void add_entity(entity_uuid uuid, boundary_t boundary, oob_policy oob = oob_policy::assert_in_bounds);
		// remove a bounded entity from the simulation.
		void remove_entity(entity_uuid uuid);
		// notify the simulation that the boundary for an existing entity has changed (e.g its moved, or its dimensions have changed)
		void notify_change(entity_uuid uuid, boundary_t new_boundary);

		// remove all entities in the simulation. world boundaries stay the same.
		void clear();
	private:
		tz::vec2 centre = {0.0f, 0.0f};
		tz::vec2 world_bounds = {0.0f, 0.0f};
		//bool dirtied_this_frame = true;
		std::unordered_map<entity_uuid, boundary_t> bounded_entities = {};
	};
}

#endif // REDNIGHTMARE_GAMELIB_PHYSICS_GRID_HIERARCHY_HPP