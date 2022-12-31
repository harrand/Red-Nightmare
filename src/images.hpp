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
		Smooth_Normals,
		PlayerAkhara_Dead,
		PlayerAkhara_Down_1,
		PlayerAkhara_Down_2,
		PlayerAkhara_Down_3,
		PlayerAkhara_Dying_1,
		PlayerAkhara_Dying_2,
		PlayerAkhara_Dying_3,
		PlayerAkhara_Dying_4,
		PlayerAkhara_Dying_5,
		PlayerAkhara_Dying_6,
		PlayerAkhara_Dying_7,
		PlayerAkhara_Dying_8,
		PlayerAkhara_Idle_1,
		PlayerAkhara_Idle_2,
		PlayerAkhara_Side_1,
		PlayerAkhara_Side_2,
		PlayerAkhara_Side_3,
		PlayerAkhara_Special,
		PlayerAkhara_Up_1,
		PlayerAkhara_Up_2,
		PlayerAkhara_Up_3,
		PlayerAkhara_LowPoly_Cast0,
		PlayerAkhara_LowPoly_Cast1,
		PlayerAkhara_LowPoly_Cast2,
		PlayerAkhara_LowPoly_Cast3,
		PlayerAkhara_LowPoly_Cast4,
		PlayerAkhara_LowPoly_Cast5,
		PlayerAkhara_LowPoly_Cast6,
		PlayerAkhara_LowPoly_Cast7,
		PlayerAkhara_LowPoly_Death00,
		PlayerAkhara_LowPoly_Death01,
		PlayerAkhara_LowPoly_Death02,
		PlayerAkhara_LowPoly_Death03,
		PlayerAkhara_LowPoly_Death04,
		PlayerAkhara_LowPoly_Death05,
		PlayerAkhara_LowPoly_Death06,
		PlayerAkhara_LowPoly_Death07,
		PlayerAkhara_LowPoly_Death08,
		PlayerAkhara_LowPoly_Death09,
		PlayerAkhara_LowPoly_Death10,
		PlayerAkhara_LowPoly_Death11,
		PlayerAkhara_LowPoly_Down0,
		PlayerAkhara_LowPoly_Down1,
		PlayerAkhara_LowPoly_Down2,
		PlayerAkhara_LowPoly_Down3,
		PlayerAkhara_LowPoly_EndCast0,
		PlayerAkhara_LowPoly_EndCast1,
		PlayerAkhara_LowPoly_EndCast2,
		PlayerAkhara_LowPoly_EndCast3,
		PlayerAkhara_LowPoly_EndCast4,
		PlayerAkhara_LowPoly_EndCast5,
		PlayerAkhara_LowPoly_Idle0,
		PlayerAkhara_LowPoly_Idle1,
		PlayerAkhara_LowPoly_Side0,
		PlayerAkhara_LowPoly_Side1,
		PlayerAkhara_LowPoly_Side2,
		PlayerAkhara_LowPoly_Side3,
		PlayerAkhara_LowPoly_Up0,
		PlayerAkhara_LowPoly_Up1,
		PlayerAkhara_LowPoly_Up2,
		PlayerAkhara_LowPoly_Up3,
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
		GhostMJZombie_Death_0,
		GhostMJZombie_Death_1,
		GhostMJZombie_Death_2,
		GhostMJZombie_Death_3,
		GhostMJZombie_Down_0,
		GhostMJZombie_Down_1,
		GhostMJZombie_Down_2,
		GhostMJZombie_Down_3,
		GhostMJZombie_Idle_0,
		GhostMJZombie_Idle_1,
		GhostMJZombie_Side_0,
		GhostMJZombie_Side_1,
		GhostMJZombie_Side_2,
		GhostMJZombie_Side_3,
		GhostMJZombie_Up_0,
		GhostMJZombie_Up_1,
		GhostMJZombie_Up_2,
		GhostMJZombie_Up_3,
		GhostBanshee_Idle_0,
		GhostBanshee_Idle_1,
		GhostBanshee_Death_0,
		GhostBanshee_Death_1,
		GhostBanshee_Death_2,
		GhostBanshee_Death_3,
		GhostBanshee_Death_4,
		GhostBanshee_Death_5,
		GhostBanshee_Death_6,
		GhostBanshee_Death_7,
		GhostBanshee_Death_8,
		GhostBanshee_Death_9,
		GhostBanshee_Death_10,
		GhostBanshee_Death_11,

		PlayerAkhara_Default_Fireball,
		PlayerAkhara_Default_Fireball_Particles_0,
		PlayerAkhara_Default_Fireball_Particles_1,
		PlayerAkhara_Default_Fireball_Particles_2,
		PlayerAkhara_Default_Fireball_Particles_3,
		PlayerAkhara_Default_Fireball_Particles_4,
		PlayerAkhara_Default_Fireball_Particles_5,
		PlayerAkhara_Default_Fireball_Particles_6,

		BlockBreak_0,
		BlockBreak_1,
		BlockBreak_2,
		BlockBreak_3,
		BlockBreak_4,
		BlockBreak_5,
		BlockBreak_6,
		BlockBreak_7,
		BlockBreak_8,
		BlockBreak_9,
		BlockBreak_10,
		BlockBreak_11,
		BlockBreak_12,
		BlockBreak_13,

		BloodSplatter_0,
		BloodSplatter_1,
		BloodSplatter_2,
		BloodSplatter_3,
		BloodSplatter_4,
		BloodSplatter_5,
		BloodSplatter_6,
		BloodSplatter_7,

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

		Material_Stone,
		Material_Stone_Normal,

		Gui_Healthbar_Empty,
		Scenery_Gravestone_0,
		Scenery_Gravestone_1,
		Downwards_Trapdoor,
		Interactable_Stone_Stairs_Down_NX,
		Interactable_Stone_Stairs_Up_NX,
		Interactable_Stone_Stairs_Down_PY,
		Interactable_Stone_Stairs_Up_PY,

		Powerup_Sprint_0,
		Powerup_Sprint_1,
		Powerup_Sprint_2,

		DevLevel1_Backdrop,
		Grass_Generic_Backdrop,
		Snow_Generic_Backdrop,
		Dungeon_Floor_Generic_Backdrop,
		Dungeon_Floor_Generic_Backdrop_Normal,

		Count
	};

	tz::gl::ImageResource load_image(TextureID texid);
	tz::gl::ImageResource load_image_data(std::string_view img_file_data);
	TextureID get_normal_map_for(TextureID tex);
}

#endif // REDNIGHTMARE_IMAGES_HPP
