#include "gamelib/render/camera.hpp"
#include "tz/tz.hpp"
#include "tz/core/matrix_transform.hpp"

namespace rnlib
{
	tz::mat4 camera::view() const
	{
		return tz::view(this->position.with_more(0.0f), tz::vec3::zero());
	}

	tz::mat4 camera::projection() const
	{
		const auto [min, max] = this->get_view_bounds();
		return tz::orthographic(min[0], max[0], max[1], min[1], 0.0f, -1.0f);
	}

	std::pair<tz::vec2, tz::vec2> camera::get_view_bounds() const
	{
		const tz::vec2ui dims = tz::window().get_dimensions();
		const auto aspect_ratio = static_cast<float>(dims[0]) / dims[1];
		return
		{
			tz::vec2{-aspect_ratio, -1.0f} * zoom,
			tz::vec2{aspect_ratio, 1.0f} * zoom
		};
	}
}
