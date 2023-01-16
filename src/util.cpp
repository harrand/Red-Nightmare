#include "util.hpp"
#include "tz/core/tz.hpp"

namespace game::util
{
	hdk::vec2 get_mouse_world_location()
	{
		auto dims = static_cast<hdk::vec2>(tz::window().get_dimensions());
		const float aspect_ratio = dims[0] / dims[1];
		hdk::vec2 mouse_pos = tz::window().get_mouse_state().mouse_position;
		mouse_pos[0] /= dims[0];
		mouse_pos[1] /= dims[1];
		mouse_pos *= 2.0f;
		mouse_pos -= hdk::vec2{1.0f, 1.0f};
		mouse_pos[1] = -mouse_pos[1];
		mouse_pos[0] *= aspect_ratio;
		return mouse_pos;
	}
}
