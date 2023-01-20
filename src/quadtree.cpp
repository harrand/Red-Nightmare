#include "quadtree.hpp"

namespace game
{
	namespace detail
	{
		Box QuadtreeHelper::compute_quadrant_box(const Box& parent_box, std::size_t quadrant_id)
		{
			tz::vec2 min = {parent_box.get_left(), parent_box.get_bottom()};
			tz::vec2 child_dimensions = parent_box.get_dimensions() / 2.0f;
			switch(quadrant_id)
			{
				/*
				 *    *-----------*
				 *    |  0  |  1  |
				 *    |     |     |
				 *    |-----|-----|
				 *    |  2  |  3  |
				 *    |     |     |
				 *    *-----------*
				 *
				 *
				 */
				case 2:
					// south west
					return {min, min + child_dimensions};
				break;
				case 3:
				{
					// south east
					tz::vec2 cmin = {min[0] + child_dimensions[0], min[1]};
					return {cmin, cmin + child_dimensions};
				}
				break;
				case 0:
				{
					// north west
					tz::vec2 cmin = {min[0], min[1] + child_dimensions[1]};
					return {cmin, cmin + child_dimensions};
				}
				break;
				case 1:
					// north east
					return {min + child_dimensions, min + child_dimensions + child_dimensions};
				break;
				default:
					tz::error("Invalid quadrant id %zu. Must be 0-3", quadrant_id);
					return {{}, {}};
				break;
			}
		}

		// Given value box (the bounding box of some element), find out whether it fits in a child of the node box. Returns nullopt if none of the children can hold it.
		std::optional<std::size_t> QuadtreeHelper::find_quadrant_id(const Box& node_box, const Box& value_box)
		{
			auto centre = node_box.get_centre();
			if(value_box.get_right() < centre[0])
			{
				// West
				if(value_box.get_bottom() > centre[1])
				{
					// north west
					return 0;
				}
				else if(value_box.get_top() <= centre[1])
				{
					// south west
					return 2;
				}
				else
				{
					return std::nullopt;
				}
			}
			else if(value_box.get_left() >= centre[0])
			{
				// east
				if(value_box.get_bottom() > centre[1])
				{
					// north east
					return 1;
				}
				else if(value_box.get_top() <= centre[1])
				{
					// south east
					return 3;
				}
				else
				{
					return std::nullopt;
				}
			}
			else
			{
				return std::nullopt;
			}
		}
	}
}
