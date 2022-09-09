#ifndef REDNIGHTMARE_BOX_HPP
#define REDNIGHTMARE_BOX_HPP
#include "tz/core/vector.hpp"

namespace game
{
	class Box
	{
		public:
			Box(tz::Vec2 min, tz::Vec2 max);
			float get_left() const;
			float get_right() const;
			float get_top() const;
			float get_bottom() const;
			tz::Vec2 get_centre() const;
			tz::Vec2 get_dimensions() const;

			bool contains(const Box& box) const;
			bool intersects(const Box& box) const;

			bool operator==(const Box& rhs) const = default;
		private:
			tz::Vec2 bottom_left;
			tz::Vec2 length;
	};
}

#endif // REDNIGHTMARE_BOX_HPP
