#ifndef REDNIGHTMARE_IMAGES_HPP
#define REDNIGHTMARE_IMAGES_HPP
#include "tz/gl/resource.hpp"

namespace game
{
	// See top of images.cpp for implementation of TextureIDs. Each textureid corresponds to a compile-time loaded png blob which is converted into an ImageResource at runtime.
	enum class TextureID : std::uint32_t
	{
		PlayerClassic_Idle_1,
		PlayerClassic_Idle_2,
		Count
	};

	tz::gl::ImageResource load_image(TextureID texid);
}

#endif // REDNIGHTMARE_IMAGES_HPP
