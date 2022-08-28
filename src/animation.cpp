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

	std::array<AnimationInfo, static_cast<int>(AnimationID::Count)> animation_storage
	{
		AnimationInfo /*Death*/
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
		AnimationInfo /*Idle*/
		{
			.frames =
			{
				TextureID::PlayerClassic_Idle_1,
				TextureID::PlayerClassic_Idle_2
			},
			.fps = 2,
			.loop = true
		},
		AnimationInfo /*MoveDown*/
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
		AnimationInfo /*MoveUp*/
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
		AnimationInfo /*MoveSide*/
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
		}
	};

	Animation play_animation(AnimationID anim)
	{
		return {animation_storage[static_cast<int>(anim)]};
	}
}
