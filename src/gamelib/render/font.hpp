#ifndef RNLIB_RENDER_FONT_HPP
#define RNLIB_RENDER_FONT_HPP
#include "tz/gl/resource.hpp"
#include <array>

namespace rnlib
{
	enum class font
	{
		lucida_sans_regular,
		_count
	};

	struct font_glyph
	{
		tz::gl::image_resource image = tz::gl::image_resource::null();
		tz::vec2 min = tz::vec2::zero();
		tz::vec2 max = tz::vec2::zero();
		float to_next = 0.0f;
	};

	struct font_data
	{
		std::array<font_glyph, (26*2)+10> glyphs;
	};

	void font_system_initialise();
	void font_system_terminate();

	font_data get_font(font f);
}

#endif // RNLIB_RENDER_FONT_HPP
