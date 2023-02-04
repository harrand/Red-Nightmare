#ifndef RNLIB_RENDER_IMAGE_HPP
#define RNLIB_RENDER_IMAGE_HPP
#include "tz/gl/resource.hpp"
#include <string_view>

namespace rnlib
{
	// safe to be called from any thread... in theory.
	tz::gl::image_resource load_image_data(std::string_view img_file_data);
}

#endif // RNLIB_RENDER_IMAGE_HPP
