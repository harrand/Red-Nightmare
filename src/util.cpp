#include "util.hpp"
#include "tz/core/tz.hpp"

namespace game::util
{
	hdk::vec2 get_mouse_world_location()
	{
		const float aspect_ratio = static_cast<float>(tz::window().get_width()) / tz::window().get_height();
		hdk::vec2 mouse_pos = static_cast<hdk::vec2>(tz::window().get_mouse_position_state().get_mouse_position());
		mouse_pos[0] /= tz::window().get_width();
		mouse_pos[1] /= tz::window().get_height();
		mouse_pos *= 2.0f;
		mouse_pos -= hdk::vec2{1.0f, 1.0f};
		mouse_pos[1] = -mouse_pos[1];
		mouse_pos[0] *= aspect_ratio;
		return mouse_pos;
	}
}
