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

	struct font_data
	{
		std::array<std::optional<tz::gl::image_resource>, (26*2)+10> images;
	};

	void font_system_initialise();
	void font_system_terminate();

	font_data get_font(font f);
}

#endif // RNLIB_RENDER_FONT_HPP
