#ifndef RNLIB_CORE_TRANSFORM_HPP
#define RNLIB_CORE_TRANSFORM_HPP
#include "tz/core/data/vector.hpp"
#include "gamelib/core/box.hpp"

namespace rnlib
{
	struct transform_t
	{
		transform_t* parent = nullptr;
		tz::vec2 local_position = tz::vec2::zero();
		tz::vec2 local_scale = tz::vec2::filled(1.0f);
		float local_rotation = 0.0f;

		tz::vec2 get_position() const;
		tz::vec2 get_scale() const;
		float get_rotation() const;

		box get_bounding_box() const;

		void dbgui();
	};
}

#endif // RNLIB_CORE_TRANSFORM_HPP
