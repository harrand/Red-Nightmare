#ifndef REDNIGHTMARE_IMAGES_HPP
#define REDNIGHTMARE_IMAGES_HPP
#include "tz/gl/resource.hpp"

namespace game
{
	// See top of images.cpp for implementation of TextureIDs. Each textureid corresponds to a compile-time loaded png blob which is converted into an ImageResource at runtime.
	enum class TextureID : std::uint32_t
	{
		Missing,
		Invisible,
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
		PlayerClassic_LowPoly_Down0,
		PlayerClassic_LowPoly_Down1,
		PlayerClassic_LowPoly_Down2,
		PlayerClassic_LowPoly_Down3,
		PlayerClassic_LowPoly_Idle0,
		PlayerClassic_LowPoly_Idle1,
		PlayerClassic_LowPoly_Side0,
		PlayerClassic_LowPoly_Side1,
		PlayerClassic_LowPoly_Side2,
		PlayerClassic_LowPoly_Side3,
		PlayerClassic_LowPoly_Up0,
		PlayerClassic_LowPoly_Up1,
		PlayerClassic_LowPoly_Up2,
		PlayerClassic_LowPoly_Up3,
		GhostZombie_Death_0,
		GhostZombie_Death_1,
		GhostZombie_Death_2,
		GhostZombie_Death_3,
		GhostZombie_Down_0,
		GhostZombie_Down_1,
		GhostZombie_Down_2,
		GhostZombie_Down_3,
		GhostZombie_Idle_0,
		GhostZombie_Idle_1,
		GhostZombie_Side_0,
		GhostZombie_Side_1,
		GhostZombie_Side_2,
		GhostZombie_Side_3,
		GhostZombie_Up_0,
		GhostZombie_Up_1,
		GhostZombie_Up_2,
		GhostZombie_Up_3,

		PlayerClassic_Default_Fireball,
		PlayerClassic_Default_Fireball_Particles_0,
		PlayerClassic_Default_Fireball_Particles_1,
		PlayerClassic_Default_Fireball_Particles_2,
		PlayerClassic_Default_Fireball_Particles_3,
		PlayerClassic_Default_Fireball_Particles_4,
		PlayerClassic_Default_Fireball_Particles_5,
		PlayerClassic_Default_Fireball_Particles_6,
		FireExplosion_0,
		FireExplosion_1,
		FireExplosion_2,
		FireExplosion_3,
		FireExplosion_4,
		FireExplosion_5,
		FireExplosion_6,
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
		Nightmare_Dying_9,
		Nightmare_Dying_10,
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
	tz::gl::ImageResource load_image_data(std::string_view img_file_data);
}

#endif // REDNIGHTMARE_IMAGES_HPP
