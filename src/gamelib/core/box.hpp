#ifndef RNLIB_CORE_BOX_HPP
#define RNLIB_CORE_BOX_HPP
#include "tz/core/data/vector.hpp"
namespace rnlib
{
	class box
	{
		public:
			box(tz::vec2 min, tz::vec2 max);
			float get_left() const;
			float get_right() const;
			float get_top() const;
			float get_bottom() const;
			tz::vec2 get_centre() const;
			tz::vec2 get_dimensions() const;

			bool contains(const box& box) const;
			bool intersects(const box& box) const;

			bool operator==(const box& rhs) const = default;
		private:
			tz::vec2 bottom_left;
			tz::vec2 length;
	};
}
#endif //RNLIB_CORE_BOX_HPP

