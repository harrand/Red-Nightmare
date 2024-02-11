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
        tz::vec2 pos = this->get_centre();
        tz::vec2 boxpos = box.get_centre();

        tz::vec2 half = this->get_extent() * 0.5f;
        tz::vec2 boxhalf = box.get_extent() * 0.5f;

        float dx = boxpos[0] - pos[0];
        float px = (boxhalf[0] + half[0]) - std::abs(dx);
        if(px <= 0.01f)
        {
            return ret;
        }

        float dy = boxpos[1] - pos[1];
        float py = (boxhalf[1] + half[0]) - std::abs(dy);
        if(py <= 0.01f)
        {
            return ret;
        }

        ret.intersecting = true;
        if(px < py)
        {
            float sx = std::copysignf(1.0f, dx);
            ret.penetration_depth = px * sx;
            ret.normal = {sx, 0.0f};
        }
        else
        {
            float sy = std::copysignf(1.0f, dy);
            ret.penetration_depth = py * sy;
            ret.normal = {0.0f, sy};
        }
        return ret;
	}

	tz::vec2 aabb::get_centre() const
	{
		return (min + max) / 2.0f;
	}

    tz::vec2 aabb::get_extent() const
    {
        tz::vec2 ret = max - min;
        ret[0] = std::abs(ret[0]);
        ret[1] = std::abs(ret[1]);
        return ret;
    }
}