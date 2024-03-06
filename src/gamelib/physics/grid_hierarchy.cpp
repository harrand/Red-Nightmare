#include "gamelib/physics/grid_hierarchy.hpp"
#include "tz/core/profile.hpp"
#include "tz/core/debug.hpp"
#include <unordered_set>

namespace game::physics
{
	grid_hierarchy::grid_hierarchy(tz::vec2 world_centre, tz::vec2 world_bounds, cell_coord cell_dimensions)
	{
		this->set_dimensions(world_centre, world_bounds, cell_dimensions);
	}

	void grid_hierarchy::set_dimensions(tz::vec2 world_centre, tz::vec2 world_bounds, cell_coord cell_dimensions)
	{
		TZ_PROFZONE("grid hierarchy - set dimensions", 0xFFBB2244);
		this->centre = world_centre;
		this->bounds = world_bounds;
		this->cell_dimensions = cell_dimensions;
		this->create_cells();
		this->cached_intersections = std::nullopt;
		if(this->bounded_entities.size())
		{
			// we need to re-add all these entities now.
			for(const auto& [uuid, boundary] : this->bounded_entities)
			{
				// adding an semi-already existing entity is a bit mental. but we assume it's safe to do so.
				this->add_entity(uuid, boundary);
			}
		}
	}

	void grid_hierarchy::add_entity(entity_uuid uuid, boundary_t boundary, oob_policy oob)
	{
		TZ_PROFZONE("grid hierarchy - add entity", 0xFFBB2244);
		cell_coord coord = this->notify_change_without_cache_changes(uuid, boundary, oob);
		if(coord == nullcoord)
		{
			return;
		}
		this->bounded_entities[uuid] = boundary;

		(void)coord;
		// todo: retroactively add to cache.
		// for now just invalidate cache entirely. it seems that recalculating intersections based on grid data is actually really cheap anyway.
		// however a performance optimisation probably exists in doing cache magic.
		this->cached_intersections = std::nullopt;
	}

	void grid_hierarchy::notify_change(entity_uuid uuid, boundary_t boundary, oob_policy oob)
	{
		TZ_PROFZONE("grid hierarchy - notify change", 0xFFBB2244);
		// remove entity and add it again.
		// no real perf cost because we need to do it anyway, and saves on code.
		this->remove_entity(uuid);
		this->bounded_entities[uuid] = boundary;
		cell_coord new_coord = this->notify_change_without_cache_changes(uuid, boundary, oob);
		if(new_coord == nullcoord)
		{
			// discard, we have to remove the uuid again even though we just set it.
			// notify_change_without_cache_changes needs an upto-date bounded_entities, so we cant just rearrange stuff.
			this->bounded_entities.erase(uuid);
			return;
		}

		(void)new_coord;
		// todo: retroactively add to cache (remove_entity already removed its old occurrences from the cache)
		// for now just invalidate cache entirely. it seems that recalculating intersections based on grid data is actually really cheap anyway.
		// however a performance optimisation probably exists in doing cache magic.
		this->cached_intersections = std::nullopt;
	}

	bool grid_hierarchy::contains(entity_uuid uuid) const
	{
		return this->bounded_entities.find(uuid) != this->bounded_entities.end();
	}

	void grid_hierarchy::remove_entity(entity_uuid uuid)
	{
		TZ_PROFZONE("grid hierarchy - remove entity", 0xFFBB2244);
		// remove from bounded entities.
		this->bounded_entities.erase(uuid);
		// go through each cells and remove all mentions of this one.
		for(auto& cell : this->cells)
		{
			std::erase(cell.contained_entities, uuid);
		}

		// same for cached intersections. remove all mention of this uuid.
		if(this->cached_intersections.has_value())
		{
			std::erase_if(this->cached_intersections.value(),
			[uuid](const auto& tuple)->bool
			{
				const auto& [uuid_a, uuid_b, manifold] = tuple;
				return uuid_a == uuid || uuid_b == uuid;
			});
		}
	}

	const intersection_data& grid_hierarchy::get_intersections()
	{
		// pair hashing nonsense. ignore this begin.
		struct pair_hash
		{
			inline std::size_t operator()(const std::pair<entity_uuid, entity_uuid> & v) const
			{
				return v.first*31+v.second;
			}
		};
		// ignore this end. real implementation begins

		if(!this->cached_intersections.has_value())
		{
			TZ_PROFZONE("grid hierarchy - get intersections", 0xFFBB2244);
			this->cached_intersections = intersection_data{};
			std::unordered_set<std::pair<entity_uuid, entity_uuid>, pair_hash> duplicate_protection;
			// skip left and right columns.
			for(std::size_t i = 1; i < (this->cell_dimensions[0] - 1); i++)
			{
				for(std::size_t j = 1; j < (this->cell_dimensions[1] - 1); j++)
				{
					TZ_PROFZONE("get intersections - collaborate cell", 0xFFAA1122);
					const auto& current_cell = this->get_cell(static_cast<cell_coord>(tz::vector<std::size_t, 2>{i, j}));
					const auto& current_objects = current_cell.contained_entities;
					// iterate on all surrounding cells, including ourself.
					for(int dx = -1; dx < 1; dx++)
					{
						for(int dy = -1; dy < 1; dy++)
						{
							TZ_PROFZONE("collaborate cell - iterate over neighbours", 0xFFAA1122);
							volatile int cell_count = current_objects.size();
							const auto& other_cell = this->get_cell(static_cast<cell_coord>(tz::vector<std::size_t, 2>{i + dx, j + dy}));
							const auto& other_objects = other_cell.contained_entities;
							// narrow phase collision.
							for(entity_uuid lhs : current_objects)
							{
								for(entity_uuid rhs : other_objects)
								{
									if(lhs == rhs)
									{
										continue;
									}
									TZ_PROFZONE("neighbour - find collisions between", 0xFFAA1122);
									const boundary_t& lhs_bound = this->bounded_entities.at(lhs);
									const boundary_t& rhs_bound = this->bounded_entities.at(rhs);
									boundary_t::manifold result = lhs_bound.intersect(rhs_bound);
									if(result.intersecting && !duplicate_protection.contains({lhs, rhs}))
									{
										this->cached_intersections.value().push_back({lhs, rhs, result});
										duplicate_protection.insert({lhs, rhs});
										duplicate_protection.insert({rhs, lhs});
									}
									// erase x intersects with x.
									std::erase_if(this->cached_intersections.value(), [](const auto& tuple)
									{
										const auto& [entity_a, entity_b, unused] = tuple;
										return entity_a == entity_b;
									});
								}
							}
						}
					}
				}
			}
		}
		return this->cached_intersections.value();
	}

	void grid_hierarchy::clear()
	{
		this->create_cells();
		this->bounded_entities.clear();
		this->cached_intersections = std::nullopt;
	}

	grid_hierarchy::cell_coord grid_hierarchy::notify_change_without_cache_changes(entity_uuid uuid, boundary_t boundary, oob_policy oob = oob_policy::assert_in_bounds)
	{
		tz::vec2 boundary_centre = boundary.get_centre();

		// note: if the boundary is too big (i.e bigger than a whole cell size - we need to resize the whole thing and recalculate)
		// as our broad-phase is all about a cell sampling against all neighbour cells. if a boundary in a cell is more than a cell size, it could in theory intersect with non-neighbour boundaries. but such collisions would be ignored in this case.
		tz::vec2 extent = boundary.get_extent();
		tz::vec2 cell_size = this->bounds;
		cell_size[0] /= this->cell_dimensions[0];
		cell_size[1] /= this->cell_dimensions[1];
		if(extent[0] > cell_size[0] || extent[1] > cell_size[1])
		{
			cell_coord new_cell_max = this->cell_dimensions / 2;
			if(new_cell_max[0] < 3 || new_cell_max[1] < 3)
			{
				// remember how we iterate - only neighbours and ourselves, and skip edges of the grid.
				// if we're 3x3 for example then only the first cell is checked with its neighbours, coz theres one cell in the mdidle and the rest are edges
				// if we're less than 3x3, e.g 3x2, 2x2 or 2x1, then there are *only* edge neighbours, meaning collisions dont happen anymore at all.
				// basically, the cells should never get that massive anyway.
				tz::error("Cell size has expanded so large that the grid is now smaller than 3x3, meaning the collision detection algorithm will now fail.");
			}
			this->set_dimensions(this->centre, this->bounds, new_cell_max);
		}

		tz::vec2 grid_min = this->centre - (this->bounds * 0.5f);
		tz::vec2 grid_max = this->centre + (this->bounds * 0.5f);
		switch(oob)
		{
			case oob_policy::assert_in_bounds:
				tz::assert(grid_min[0] <= boundary_centre[0] && boundary_centre[0] <= grid_max[0], "x-coord of entity out of range. %.2f <= %.2f <= %.2f", grid_min[0], boundary_centre[0], grid_max[0]);
				tz::assert(grid_min[1] <= boundary_centre[1] && boundary_centre[1] <= grid_max[1], "y-coord of entity out of range. %.2f <= %.2f <= %.2f", grid_min[1], boundary_centre[1], grid_max[1]);
			break;
			case oob_policy::discard:
				if(grid_min[0] > boundary_centre[0] || boundary_centre[0] > grid_max[0]){return nullcoord;}
				if(grid_min[1] > boundary_centre[1] || boundary_centre[1] > grid_max[1]){return nullcoord;}
			break;
			default:
				tz::error("Grid hierarchy OOB Policy unsupported. Either you invoked an unimplemented code path or the policy variable is garbage.");
			break;
		}
		tz::vec2 offseted_centre = boundary_centre - this->centre;
		// offseted centre is now relative to the centre of the grid.
		offseted_centre += this->bounds * 0.5f;
		// offseted centre is now between {0.0} and {bounds}
		// sanity checks. these should always pass unless the impl is wrong.
		tz::assert(offseted_centre[0] >= 0.0f && offseted_centre[0] <= this->bounds[0]);
		tz::assert(offseted_centre[1] >= 0.0f && offseted_centre[1] <= this->bounds[1]);
		// divide by bounds.
		offseted_centre[0] /= this->bounds[0];
		offseted_centre[1] /= this->bounds[1];
		// between {0.0, 0.0} and {1.0, 1.0}
		offseted_centre[0] *= this->cell_dimensions[0];
		offseted_centre[1] *= this->cell_dimensions[1];
		// between {0.0, 0.0} and cell_dimensions
		cell_coord coord = {static_cast<unsigned short>(offseted_centre[0]), static_cast<unsigned short>(offseted_centre[1])};
		this->get_cell(coord).contained_entities.push_back(uuid);

		return coord;
	}


	void grid_hierarchy::create_cells()
	{
		TZ_PROFZONE("grid hierarchy - create cells", 0xFFBB2244);
		const std::size_t cell_count = static_cast<std::size_t>(this->cell_dimensions[0]) * this->cell_dimensions[1];
		this->cells.clear();
		this->cells.resize(cell_count);
		for(unsigned short x = 0; x < this->cell_dimensions[0]; x++)
		{
			for(unsigned short y = 0; y < this->cell_dimensions[1]; y++)
			{
				cell_t& cell = this->get_cell({x, y});
				cell.coord = {x, y};
				cell.contained_entities.clear();
			}
		}
	}

	const grid_hierarchy::cell_t& grid_hierarchy::get_cell(cell_coord coord) const
	{
		tz::assert(coord[0] < this->cell_dimensions[0] && coord[1] < this->cell_dimensions[1]);
		return this->cells[coord[0] + (coord[1] * this->cell_dimensions[0])];
	}

	grid_hierarchy::cell_t& grid_hierarchy::get_cell(cell_coord coord)
	{
		tz::assert(coord[0] < this->cell_dimensions[0] && coord[1] < this->cell_dimensions[1]);
		return this->cells[coord[0] + (coord[1] * this->cell_dimensions[0])];
	}
}