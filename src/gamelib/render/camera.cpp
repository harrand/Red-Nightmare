#include "gamelib/render/camera.hpp"
#include "tz/tz.hpp"
#include "tz/core/matrix_transform.hpp"

namespace rnlib
{
	tz::mat4 camera::view() const
	{
		const tz::vec2ui dims = tz::window().get_dimensions();
		const auto aspect_ratio = static_cast<float>(dims[0]) / dims[1];
		return tz::orthographic(-aspect_ratio * this->zoom, aspect_ratio * this->zoom, zoom, -zoom, 0.0f, -1.0f);
	}
}
