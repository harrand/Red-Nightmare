#ifndef REDNIGHTMARE_IMAGES_HPP
#define REDNIGHTMARE_IMAGES_HPP
#include "tz/gl/resource.hpp"

namespace game
{
	// See top of images.cpp for implementation of TextureIDs. Each textureid corresponds to a compile-time loaded png blob which is converted into an ImageResource at runtime.
	enum class TextureID : std::uint32_t
	{
		Missing,
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
		PlayerClassic_Dying_7,
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
		Nightmare_Dead,
		Nightmare_Down_1,
		Nightmare_Down_2,
		Nightmare_Down_3,
		Nightmare_Dying_1,
		Nightmare_Dying_2,
		Nightmare_Dying_3,
		Nightmare_Dying_4,
		Nightmare_Dying_5,
		Nightmare_Dying_6,
		Nightmare_Dying_7,
		Nightmare_Dying_8,
		Nightmare_Idle_1,
		Nightmare_Idle_2,
		Nightmare_Side_1,
		Nightmare_Side_2,
		Nightmare_Side_3,
		Nightmare_Special,
		Nightmare_Up_1,
		Nightmare_Up_2,
		Nightmare_Up_3,
		Count
	};

	tz::gl::ImageResource load_image(TextureID texid);
}

#endif // REDNIGHTMARE_IMAGES_HPP
