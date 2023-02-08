#ifndef RNLIB_RENDER_IMAGE_HPP
#define RNLIB_RENDER_IMAGE_HPP
#include "tz/gl/resource.hpp"
#include <string_view>

namespace rnlib
{
	// safe to be called from any thread... in theory.
	tz::gl::image_resource load_image_data(std::string_view img_file_data);

	using image_id_t = std::uint32_t;
	namespace image_id
	{
		enum image_id_e : image_id_t
		{
			undefined,
			invisible,
			_count
		};
	}

	tz::gl::image_resource create_image(image_id_t iid);
}

#endif // RNLIB_RENDER_IMAGE_HPP
