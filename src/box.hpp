#ifndef REDNIGHTMARE_BOX_HPP
#define REDNIGHTMARE_BOX_HPP
#include "tz/core/data/vector.hpp"

namespace game
{
	class Box
	{
		public:
			Box(tz::vec2 min, tz::vec2 max);
			float get_left() const;
			float get_right() const;
			float get_top() const;
			float get_bottom() const;
			tz::vec2 get_centre() const;
			tz::vec2 get_dimensions() const;

			bool contains(const Box& box) const;
			bool intersects(const Box& box) const;

			bool operator==(const Box& rhs) const = default;
		private:
			tz::vec2 bottom_left;
			tz::vec2 length;
	};
}

#endif // REDNIGHTMARE_BOX_HPP
