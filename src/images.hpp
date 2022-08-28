#ifndef REDNIGHTMARE_IMAGES_HPP
#define REDNIGHTMARE_IMAGES_HPP
#include "tz/gl/resource.hpp"

namespace game
{
	// See top of images.cpp for implementation of TextureIDs. Each textureid corresponds to a compile-time loaded png blob which is converted into an ImageResource at runtime.
	enum class TextureID : std::uint32_t
	{
		PlayerClassic_Dead,
		PlayerClassic_Down_1,
		PlayerClassic_Down_2,
		PlayerClassic_Down_3,
		PlayerClassic_Dying_1,
		PlayerClassic_Dying_2,
		PlayerClassic_Dying_3,
		PlayerClassic_Dying_4,
		PlayerClassic_Dying_5,
		PlayerClassic_Dying_6,
		PlayerCLassic_Dying_7,
		PlayerClassic_Dying_8,
		PlayerClassic_Idle_1,
		PlayerClassic_Idle_2,
		PlayerClassic_Side_1,
		PlayerClassic_Side_2,
		PlayerClassic_Side_3,
		PlayerClassic_Special,
		PlayerClassic_Up_1,
		PlayerClassic_Up_2,
		PlayerClassic_Up_3,
		Count
	};

	tz::gl::ImageResource load_image(TextureID texid);
}

#endif // REDNIGHTMARE_IMAGES_HPP
