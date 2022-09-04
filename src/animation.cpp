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
		return this->info.frames.size() * (1000 / this->info.fps);
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

		std::size_t frame_id = std::clamp(elapsed_millis / frame_period_millis, 0ull, this->info.frames.size());
		return this->info.frames[frame_id];
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
		AnimationInfo /*PlayerClassic_Death*/
		{
			.frames =
			{
				TextureID::PlayerClassic_Dying_1,
				TextureID::PlayerClassic_Dying_2,
				TextureID::PlayerClassic_Dying_3,
				TextureID::PlayerClassic_Dying_4,
				TextureID::PlayerClassic_Dying_5,
				TextureID::PlayerClassic_Dying_6,
				TextureID::PlayerClassic_Dying_6,
				TextureID::PlayerClassic_Dying_6,
				TextureID::PlayerClassic_Dying_7,
				TextureID::PlayerClassic_Dying_7,
				TextureID::PlayerClassic_Dying_7,
				TextureID::PlayerClassic_Dying_7,
				TextureID::PlayerClassic_Dying_7,
				TextureID::PlayerClassic_Dying_7,
				TextureID::PlayerClassic_Dying_8,
				TextureID::PlayerClassic_Dead
			},
			.fps = 8,
			.loop = false
		},
		AnimationInfo /*PlayerClassic_Idle*/
		{
			.frames =
			{
				TextureID::PlayerClassic_Idle_1,
				TextureID::PlayerClassic_Idle_2
			},
			.fps = 2,
			.loop = true
		},
		AnimationInfo /*PlayerClassic_MoveDown*/
		{
			.frames =
			{
				TextureID::PlayerClassic_Down_1,
				TextureID::PlayerClassic_Down_2,
				TextureID::PlayerClassic_Down_3,
				TextureID::PlayerClassic_Down_2
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*PlayerClassic_MoveUp*/
		{
			.frames = 
			{
				TextureID::PlayerClassic_Up_1,
				TextureID::PlayerClassic_Up_2,
				TextureID::PlayerClassic_Up_3,
				TextureID::PlayerClassic_Up_2
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*PlayerClassic_MoveSide*/
		{
			.frames = 
			{
				TextureID::PlayerClassic_Side_1,
				TextureID::PlayerClassic_Side_2,
				TextureID::PlayerClassic_Side_3,
				TextureID::PlayerClassic_Side_2
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*PlayerClassic_LowPoly_MoveDown*/
		{
			.frames = 
			{
				TextureID::PlayerClassic_LowPoly_Down0,
				TextureID::PlayerClassic_LowPoly_Down1,
				TextureID::PlayerClassic_LowPoly_Down2,
				TextureID::PlayerClassic_LowPoly_Down3
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*PlayerClassic_LowPoly_Idle*/
		{
			.frames = 
			{
				TextureID::PlayerClassic_LowPoly_Idle0,
				TextureID::PlayerClassic_LowPoly_Idle1
			},
			.fps = 1,
			.loop = true
		},
		AnimationInfo /*PlayerClassic_LowPoly_MoveSide*/
		{
			.frames = 
			{
				TextureID::PlayerClassic_LowPoly_Side0,
				TextureID::PlayerClassic_LowPoly_Side1,
				TextureID::PlayerClassic_LowPoly_Side2,
				TextureID::PlayerClassic_LowPoly_Side3
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*PlayerClassic_LowPoly_MoveUp*/
		{
			.frames = 
			{
				TextureID::PlayerClassic_LowPoly_Up0,
				TextureID::PlayerClassic_LowPoly_Up1,
				TextureID::PlayerClassic_LowPoly_Up2,
				TextureID::PlayerClassic_LowPoly_Up3
			},
			.fps = 6,
			.loop = true
		},
		AnimationInfo /*PlayerClassic_DefaultFireball_Idle*/
		{
			.frames =
			{
				TextureID::PlayerClassic_Default_Fireball
			},
			.fps = 1,
			.loop = true
		},
		AnimationInfo /*PlayerClassic_FireSmoke*/
		{
			.frames =
			{
				TextureID::PlayerClassic_Default_Fireball_Particles_0,
				TextureID::PlayerClassic_Default_Fireball_Particles_1,
				TextureID::PlayerClassic_Default_Fireball_Particles_2,
				TextureID::PlayerClassic_Default_Fireball_Particles_3,
				TextureID::PlayerClassic_Default_Fireball_Particles_4,
				TextureID::PlayerClassic_Default_Fireball_Particles_5,
				TextureID::PlayerClassic_Default_Fireball_Particles_6,
			},
			.fps = 9,
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
		}
	};

	Animation play_animation(AnimationID anim)
	{
		return {animation_storage[static_cast<int>(anim)]};
	}
}
