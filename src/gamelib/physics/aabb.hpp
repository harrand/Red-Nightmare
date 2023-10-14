#ifndef RN_GAMELIB_PHYSICS_AABB_HPP
#define RN_GAMELIB_PHYSICS_AABB_HPP
#include "tz/core/data/vector.hpp"
namespace game::physics
{
	class aabb
	{
		public:
			aabb(tz::vec2 min, tz::vec2 max);
			float get_left() const;
			float get_right() const;
			float get_top() const;
			float get_bottom() const;
			tz::vec2 get_centre() const;
			tz::vec2 get_dimensions() const;

			bool contains(const aabb& rhs) const;
			bool intersects(const aabb& rhs) const;

			bool operator==(const aabb& rhs) const = default;
		private:
			tz::vec2 bottom_left;
			tz::vec2 length;
	};
}
#endif //RN_GAMELIB_PHYSICS_AABB_HPP