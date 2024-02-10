#ifndef REDNIGHTMARE_GAMELIB_PHYSICS_AABB_HPP
#define REDNIGHTMARE_GAMELIB_PHYSICS_AABB_HPP
#include "tz/core/data/vector.hpp"

namespace game::physics
{
	struct aabb
	{
		struct manifold
		{
			bool intersecting = false;
			tz::vec2 normal;
			float penetration_depth;
		};

		tz::vec2 min, max;

		manifold intersect(const tz::vec2& point) const;
		manifold intersect(const aabb& box) const;
		tz::vec2 get_centre() const;
	};
}

#endif // REDNIGHTMARE_GAMELIB_PHYSICS_AABB_HPP