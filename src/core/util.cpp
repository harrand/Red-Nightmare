#include "core/util.hpp"
#include "tz/tz.hpp"

namespace game::util
{
	tz::vec2 get_mouse_world_location()
	{
		auto dims = static_cast<tz::vec2>(tz::window().get_dimensions());
		const float aspect_ratio = dims[0] / dims[1];
		tz::vec2 mouse_pos = tz::window().get_mouse_state().mouse_position;
		mouse_pos[0] /= dims[0];
		mouse_pos[1] /= dims[1];
		mouse_pos *= 2.0f;
		mouse_pos -= tz::vec2{1.0f, 1.0f};
		mouse_pos[1] = -mouse_pos[1];
		mouse_pos[0] *= aspect_ratio;
		return mouse_pos;
	}
}
