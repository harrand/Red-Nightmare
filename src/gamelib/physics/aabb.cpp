#include "gamelib/physics/aabb.hpp"

namespace game::physics
{
	aabb::manifold aabb::intersect(const tz::vec2& point) const
	{
		aabb::manifold ret;
		if(	point[0] >= min[0] && point[0] <= max[0] &&
        	point[1] >= min[1] && point[1] <= max[1])
		{
			ret.intersecting = true;
			// Calculate normal pointing from AABB center to the point
            tz::vec2 centre = {(min[0] + max[0]) / 2.0f, (min[1] + max[1]) / 2.0f};
            ret.normal = {point[0] - centre[0], point[1] - centre[1]};

            // Calculate penetration depth as the distance from the point to the nearest edge of the AABB
            float left_dist = point[0] - min[0];
            float right_dist = max[0] - point[0];
            float top_dist = max[1] - point[1];
            float bottom_dist = point[1] - min[1];

            float min_dist = std::min({left_dist, right_dist, top_dist, bottom_dist});
            ret.penetration_depth = min_dist;
		}
		return ret;
	}

	aabb::manifold aabb::intersect(const aabb& box) const
	{
		aabb::manifold ret;
		bool x_overlap = (max[0] >= box.min[0] && min[0] <= box.max[0]);
        bool y_overlap = (max[1] >= box.min[1] && min[1] <= box.max[1]);

        if (x_overlap && y_overlap) {
            // Calculate penetration depths along both axes
            float x_penetration = std::min(max[0] - box.min[0], box.max[0] - min[0]);
            float y_penetration = std::min(max[1] - box.min[1], box.max[1] - min[1]);

            // Determine the axis with the minimum penetration depth
            if (x_penetration < y_penetration) {
                ret.normal = {1.0f, 0.0f};
                ret.penetration_depth = x_penetration;
            } else {
                ret.normal = {0.0f, 1.0f};
                ret.penetration_depth = y_penetration;
            }
            ret.intersecting = true;
        }
        
        return ret;
	}

	tz::vec2 aabb::get_centre() const
	{
		return (min + max) / 2.0f;
	}
}