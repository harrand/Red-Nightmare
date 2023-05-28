#ifndef RNLIB_RENDER_CAMERA_HPP
#define RNLIB_RENDER_CAMERA_HPP
#include "tz/core/data/vector.hpp"
#include "tz/core/matrix.hpp"

namespace rnlib
{
	struct camera
	{
		tz::vec2 position = tz::vec2::zero();
		float zoom = 1.0f;

		tz::mat4 view() const;
		tz::mat4 projection() const;
		tz::vec2 screen_to_world(tz::vec2 screen_space_position) const;
		std::pair<tz::vec2, tz::vec2> get_view_bounds() const;
		bool operator==(const camera& rhs) const = default;
	};
}

#endif // RNLIB_RENDER_CAMERA_HPP
