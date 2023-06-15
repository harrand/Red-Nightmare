#ifndef RNLIB_CORE_TRANSFORM_HPP
#define RNLIB_CORE_TRANSFORM_HPP
#include "tz/core/data/vector.hpp"
#include "gamelib/core/box.hpp"

namespace rnlib
{
	struct transform_t
	{
		tz::vec2 position = tz::vec2::zero();
		tz::vec2 scale = tz::vec2::filled(1.0f);
		float rotation = 0.0f;

		box get_bounding_box() const;

		void dbgui();
		transform_t operator+(const transform_t& rhs) const;
	};
}

#endif // RNLIB_CORE_TRANSFORM_HPP
