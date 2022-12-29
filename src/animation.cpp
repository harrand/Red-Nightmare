#include "animation.hpp"

namespace game
{
	Animation::Animation(AnimationInfo info):
	info(info),
	begin(tz::system_time())
	{

	}

	unsigned int Animation::length_millis() const
	{
		return this->info.frames.size() * (1000 / (this->info.fps * this->fps_multiplier));
	}

	bool Animation::complete() const
	{
		auto elapsed_millis = (tz::system_time() - this->begin).millis<unsigned long long>();
		auto length = this->length_millis();
		return elapsed_millis >= length;
	}

	const AnimationInfo& Animation::get_info() const
	{
		return this->info;
	}

	TextureID Animation::get_texture()
	{
		hdk::assert(!this->info.frames.empty(), "No frames in animation");
		auto now = tz::system_time().millis<unsigned long long>();
		auto begin_millis = this->begin.millis<unsigned long long>();
		auto elapsed_millis = now - begin_millis;
		if(elapsed_millis >= this->length_millis())
		{
			if(this->info.loop)
			{
				this->begin = tz::system_time();
			}
			else
			{
				return this->info.frames.back();
			}
		}

		// Get number of millis per frame.
		unsigned int frame_period_millis = this->length_millis() / this->info.frames.size();
		// frameid * frame_period_millis = elapsed_time

		std::size_t frame_id = std::clamp(elapsed_millis / frame_period_millis, 0ull, this->info.frames.size() - 1);
		return this->info.frames[frame_id];
	}

	void Animation::set_fps_multiplyer(float mul)
	{
		this->fps_multiplier = mul;
	}

	float Animation::get_fps_multiplyer() const
	{
		return this->fps_multiplier;
	}

	bool Animation::operator==(const Animation& rhs) const
	{
		return this->info == rhs.info;
	}

	std::array<AnimationInfo, static_cast<int>(AnimationID::Count)> animation_storage
	{
		AnimationInfo /*Missing*/
		{
			.frames =
			{
				TextureID::Missing
			},
			.fps = 1,
			.loop = false
		},
		AnimationInfo /*Invisible*/
		{
			.frames =
			{
				TextureID::Invisible
			},
			.fps = 1,
			.loop = false
		},
		AnimationInfo /*PlayerAkhara_Death*/
		{
			.frames =
			{
				TextureID::PlayerAkhara_Dying_1,
				TextureID::PlayerAkhara_Dying_2,
				TextureID::PlayerAkhara_Dying_3,
				TextureID::PlayerAkhara_Dying_4,
				TextureID::PlayerAkhara_Dying_5,
				TextureID::PlayerAkhara_Dying_6,
				TextureID::PlayerAkhara_Dying_6,
				TextureID::PlayerAkhara_Dying_6,
				TextureID::PlayerAkhara_Dying_7,
				TextureID::PlayerAkhara_Dying_7,
				TextureID::PlayerAkhara_Dying_7,
				TextureID::PlayerAkhara_Dying_7,
				TextureID::PlayerAkhara_Dying_7,
				TextureID::PlayerAkhara_Dying_7,
				TextureID::PlayerAkhara_Dying_8,
				TextureID::PlayerAkhara_Dead
			},
			.fps = 8,
			.loop = false
		},
		AnimationInfo /*PlayerAkhara_Idle*/
		{
			.frames =
			{
				TextureID::PlayerAkhara_Idle_1,
				TextureID::PlayerAkhara_Idle_2
			},
			.fps = 2,
			.loop = true
		},
		AnimationInfo /*PlayerAkhara_MoveDown*/
		{
			.frames =
			{
				TextureID::PlayerAkhara_Down_1,
				TextureID::PlayerAkhara_Down_2,
				TextureID::PlayerAkhara_Down_3,
				TextureID::PlayerAkhara_Down_2
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*PlayerAkhara_MoveUp*/
		{
			.frames = 
			{
				TextureID::PlayerAkhara_Up_1,
				TextureID::PlayerAkhara_Up_2,
				TextureID::PlayerAkhara_Up_3,
				TextureID::PlayerAkhara_Up_2
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*PlayerAkhara_MoveSide*/
		{
			.frames = 
			{
				TextureID::PlayerAkhara_Side_1,
				TextureID::PlayerAkhara_Side_2,
				TextureID::PlayerAkhara_Side_3,
				TextureID::PlayerAkhara_Side_2
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*PlayerAkhara_LowPoly_Cast*/
		{
			.frames = 
			{
				TextureID::PlayerAkhara_LowPoly_Cast0,
				TextureID::PlayerAkhara_LowPoly_Cast1,
				TextureID::PlayerAkhara_LowPoly_Cast2,
				TextureID::PlayerAkhara_LowPoly_Cast3,
				TextureID::PlayerAkhara_LowPoly_Cast4,
				TextureID::PlayerAkhara_LowPoly_Cast5,
				TextureID::PlayerAkhara_LowPoly_Cast6,
				TextureID::PlayerAkhara_LowPoly_Cast7,
			},
			.fps = 7,
			.loop = true
		},
		AnimationInfo /*PlayerAkhara_LowPoly_Death*/
		{
			.frames = 
			{
				TextureID::PlayerAkhara_LowPoly_Death00,
				TextureID::PlayerAkhara_LowPoly_Death01,
				TextureID::PlayerAkhara_LowPoly_Death02,
				TextureID::PlayerAkhara_LowPoly_Death03,
				TextureID::PlayerAkhara_LowPoly_Death04,
				TextureID::PlayerAkhara_LowPoly_Death05,
				TextureID::PlayerAkhara_LowPoly_Death06,
				TextureID::PlayerAkhara_LowPoly_Death07,
				TextureID::PlayerAkhara_LowPoly_Death08,
				TextureID::PlayerAkhara_LowPoly_Death09,
				TextureID::PlayerAkhara_LowPoly_Death10,
				TextureID::PlayerAkhara_LowPoly_Death11,
			},
			.fps = 14,
			.loop = false
		},
		AnimationInfo /*PlayerAkhara_LowPoly_MoveDown*/
		{
			.frames = 
			{
				TextureID::PlayerAkhara_LowPoly_Down0,
				TextureID::PlayerAkhara_LowPoly_Down1,
				TextureID::PlayerAkhara_LowPoly_Down2,
				TextureID::PlayerAkhara_LowPoly_Down3
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*PlayerAkhara_LowPoly_EndCast*/
		{
			.frames = 
			{
				TextureID::PlayerAkhara_LowPoly_EndCast0,
				TextureID::PlayerAkhara_LowPoly_EndCast1,
				TextureID::PlayerAkhara_LowPoly_EndCast2,
				TextureID::PlayerAkhara_LowPoly_EndCast3,
				TextureID::PlayerAkhara_LowPoly_EndCast4
			},
			.fps = 12,
			.loop = false
		},
		AnimationInfo /*PlayerAkhara_LowPoly_Idle*/
		{
			.frames = 
			{
				TextureID::PlayerAkhara_LowPoly_Idle0,
				TextureID::PlayerAkhara_LowPoly_Idle1
			},
			.fps = 2,
			.loop = true
		},
		AnimationInfo /*PlayerAkhara_LowPoly_MoveSide*/
		{
			.frames = 
			{
				TextureID::PlayerAkhara_LowPoly_Side0,
				TextureID::PlayerAkhara_LowPoly_Side1,
				TextureID::PlayerAkhara_LowPoly_Side2,
				TextureID::PlayerAkhara_LowPoly_Side3
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*PlayerAkhara_LowPoly_MoveUp*/
		{
			.frames = 
			{
				TextureID::PlayerAkhara_LowPoly_Up0,
				TextureID::PlayerAkhara_LowPoly_Up1,
				TextureID::PlayerAkhara_LowPoly_Up2,
				TextureID::PlayerAkhara_LowPoly_Up3
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*GhostZombie_Death*/
		{
			.frames = 
			{
				TextureID::GhostZombie_Death_0,
				TextureID::GhostZombie_Death_1,
				TextureID::GhostZombie_Death_2,
				TextureID::GhostZombie_Death_3
			},
			.fps = 14,
			.loop = false
		},
		AnimationInfo /*GhostZombie_MoveDown*/
		{
			.frames = 
			{
				TextureID::GhostZombie_Down_0,
				TextureID::GhostZombie_Down_1,
				TextureID::GhostZombie_Down_2,
				TextureID::GhostZombie_Down_3
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*GhostZombie_Idle*/
		{
			.frames = 
			{
				TextureID::GhostZombie_Idle_0,
				TextureID::GhostZombie_Idle_1
			},
			.fps = 2,
			.loop = true
		},
		AnimationInfo /*GhostZombie_MoveSide*/
		{
			.frames = 
			{
				TextureID::GhostZombie_Side_0,
				TextureID::GhostZombie_Side_1,
				TextureID::GhostZombie_Side_2,
				TextureID::GhostZombie_Side_3
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*GhostZombie_MoveUp*/
		{
			.frames = 
			{
				TextureID::GhostZombie_Up_0,
				TextureID::GhostZombie_Up_1,
				TextureID::GhostZombie_Up_2,
				TextureID::GhostZombie_Up_3
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*GhostMJZombie_Death*/
		{
			.frames = 
			{
				TextureID::GhostMJZombie_Death_0,
				TextureID::GhostMJZombie_Death_1,
				TextureID::GhostMJZombie_Death_2,
				TextureID::GhostMJZombie_Death_3
			},
			.fps = 14,
			.loop = false
		},
		AnimationInfo /*GhostMJZombie_MoveDown*/
		{
			.frames = 
			{
				TextureID::GhostMJZombie_Down_0,
				TextureID::GhostMJZombie_Down_1,
				TextureID::GhostMJZombie_Down_2,
				TextureID::GhostMJZombie_Down_3
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*GhostMJZombie_Idle*/
		{
			.frames = 
			{
				TextureID::GhostMJZombie_Idle_0,
				TextureID::GhostMJZombie_Idle_1
			},
			.fps = 2,
			.loop = true
		},
		AnimationInfo /*GhostMJZombie_MoveSide*/
		{
			.frames = 
			{
				TextureID::GhostMJZombie_Side_0,
				TextureID::GhostMJZombie_Side_1,
				TextureID::GhostMJZombie_Side_2,
				TextureID::GhostMJZombie_Side_3
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*GhostMJZombie_MoveUp*/
		{
			.frames = 
			{
				TextureID::GhostMJZombie_Up_0,
				TextureID::GhostMJZombie_Up_1,
				TextureID::GhostMJZombie_Up_2,
				TextureID::GhostMJZombie_Up_3
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*GhostBanshee_Idle*/
		{
			.frames = 
			{
				TextureID::GhostBanshee_Idle_0,
				TextureID::GhostBanshee_Idle_1,
			},
			.fps = 2,
			.loop = true
		},
		AnimationInfo /*GhostBanshee_Death*/
		{
			.frames = 
			{
				TextureID::GhostBanshee_Death_0,
				TextureID::GhostBanshee_Death_1,
				TextureID::GhostBanshee_Death_2,
				TextureID::GhostBanshee_Death_3,
				TextureID::GhostBanshee_Death_4,
				TextureID::GhostBanshee_Death_5,
				TextureID::GhostBanshee_Death_6,
				TextureID::GhostBanshee_Death_7,
				TextureID::GhostBanshee_Death_8,
				TextureID::GhostBanshee_Death_9,
				TextureID::GhostBanshee_Death_10,
				TextureID::GhostBanshee_Death_11,
			},
			.fps = 14,
			.loop = false
		},
		AnimationInfo /*PlayerAkhara_DefaultFireball_Idle*/
		{
			.frames =
			{
				TextureID::PlayerAkhara_Default_Fireball
			},
			.fps = 1,
			.loop = true
		},
		AnimationInfo /*PlayerAkhara_FireSmoke*/
		{
			.frames =
			{
				TextureID::PlayerAkhara_Default_Fireball_Particles_0,
				TextureID::PlayerAkhara_Default_Fireball_Particles_1,
				TextureID::PlayerAkhara_Default_Fireball_Particles_2,
				TextureID::PlayerAkhara_Default_Fireball_Particles_3,
				TextureID::PlayerAkhara_Default_Fireball_Particles_4,
				TextureID::PlayerAkhara_Default_Fireball_Particles_5,
				TextureID::PlayerAkhara_Default_Fireball_Particles_6,
			},
			.fps = 9,
			.loop = false
		},
		AnimationInfo /*BlockBreak*/
		{
			.frames =
			{
				TextureID::BlockBreak_0,
				TextureID::BlockBreak_1,
				TextureID::BlockBreak_2,
				TextureID::BlockBreak_3,
				TextureID::BlockBreak_4,
				TextureID::BlockBreak_5,
				TextureID::BlockBreak_6,
				TextureID::BlockBreak_7,
				TextureID::BlockBreak_8,
				TextureID::BlockBreak_9,
				TextureID::BlockBreak_10,
				TextureID::BlockBreak_11,
				TextureID::BlockBreak_12,
				TextureID::BlockBreak_13,
			},
			.fps = 24,
			.loop = false
		},
		AnimationInfo /*BloodSplatter*/
		{
			.frames =
			{
				TextureID::BloodSplatter_0,
				TextureID::BloodSplatter_1,
				TextureID::BloodSplatter_2,
				TextureID::BloodSplatter_3,
				TextureID::BloodSplatter_4,
				TextureID::BloodSplatter_5,
				TextureID::BloodSplatter_6,
				TextureID::BloodSplatter_7
			},
			.fps = 16,
			.loop = false
		},
		AnimationInfo /*PlayerAkhara_FireExplosion*/
		{
			.frames =
			{
				TextureID::FireExplosion_0,
				TextureID::FireExplosion_1,
				TextureID::FireExplosion_2,
				TextureID::FireExplosion_3,
				TextureID::FireExplosion_4,
				TextureID::FireExplosion_5,
				TextureID::FireExplosion_6,
			},
			.fps = 11,
			.loop = false
		},
		AnimationInfo /*Nightmare_Spawn*/
		{
			.frames =
			{
				TextureID::Nightmare_Dead,
				TextureID::Nightmare_Dying_10,
				TextureID::Nightmare_Dying_9,
				TextureID::Nightmare_Dying_8,
				TextureID::Nightmare_Dying_7,
				TextureID::Nightmare_Dying_6,
				TextureID::Nightmare_Dying_5,
				TextureID::Nightmare_Dying_4,
				TextureID::Nightmare_Dying_3,
				TextureID::Nightmare_Dying_2,
				TextureID::Nightmare_Dying_1
			},
			.fps = 16,
			.loop = false
		},
		AnimationInfo /*Nightmare_Death*/
		{
			.frames =
			{
				TextureID::Nightmare_Dying_1,
				TextureID::Nightmare_Dying_2,
				TextureID::Nightmare_Dying_3,
				TextureID::Nightmare_Dying_4,
				TextureID::Nightmare_Dying_5,
				TextureID::Nightmare_Dying_6,
				TextureID::Nightmare_Dying_7,
				TextureID::Nightmare_Dying_8,
				TextureID::Nightmare_Dying_9,
				TextureID::Nightmare_Dying_10,
				TextureID::Nightmare_Dead
			},
			.fps = 16,
			.loop = false
		},
		AnimationInfo /*Nightmare_Idle*/
		{
			.frames =
			{
				TextureID::Nightmare_Idle_1,
				TextureID::Nightmare_Idle_2
			},
			.fps = 2,
			.loop = true
		},
		AnimationInfo /*Nightmare_MoveDown*/
		{
			.frames =
			{
				TextureID::Nightmare_Down_1,
				TextureID::Nightmare_Down_2,
				TextureID::Nightmare_Down_3,
				TextureID::Nightmare_Down_2
			},
			.fps = 9,
			.loop = true
		},
		AnimationInfo /*Nightmare_MoveUp*/
		{
			.frames = 
			{
				TextureID::Nightmare_Up_1,
				TextureID::Nightmare_Up_2,
				TextureID::Nightmare_Up_3,
				TextureID::Nightmare_Up_2
			},
			.fps = 9,
			.loop = true
		},
		AnimationInfo /*Nightmare_MoveSide*/
		{
			.frames = 
			{
				TextureID::Nightmare_Side_1,
				TextureID::Nightmare_Side_2,
				TextureID::Nightmare_Side_3,
				TextureID::Nightmare_Side_2
			},
			.fps = 9,
			.loop = true
		},
		AnimationInfo /*Material_Stone*/
		{
			.frames = 
			{
				TextureID::Material_Stone,
			},
			.fps = 1,
			.loop = false
		},
		AnimationInfo /*Gui_Healthbar_Empty*/
		{
			.frames = 
			{
				TextureID::Gui_Healthbar_Empty,
			},
			.fps = 1,
			.loop = false
		},
		AnimationInfo /*Scenery_Gravestone_0*/
		{
			.frames = 
			{
				TextureID::Scenery_Gravestone_0,
			},
			.fps = 1,
			.loop = false
		},
		AnimationInfo /*Scenery_Gravestone_1*/
		{
			.frames = 
			{
				TextureID::Scenery_Gravestone_1,
			},
			.fps = 1,
			.loop = false
		},
		AnimationInfo /*Interactable_Stone_Stairs_Down_PY*/
		{
			.frames = 
			{
				TextureID::Interactable_Stone_Stairs_Down_PY,
			},
			.fps = 1,
			.loop = false
		},
		AnimationInfo /*Interactable_Stone_Stairs_Up_PY*/
		{
			.frames = 
			{
				TextureID::Interactable_Stone_Stairs_Up_PY,
			},
			.fps = 1,
			.loop = false
		},
		AnimationInfo /*Powerup_Sprint*/
		{
			.frames = 
			{
				TextureID::Powerup_Sprint_0,
				TextureID::Powerup_Sprint_1,
				TextureID::Powerup_Sprint_2
			},
			.fps = 5,
			.loop = true
		}
	};

	Animation play_animation(AnimationID anim)
	{
		return {animation_storage[static_cast<int>(anim)]};
	}
}
