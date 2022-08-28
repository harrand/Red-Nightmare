#ifndef REDNIGHTMARE_ANIMATION_HPP
#define REDNIGHTMARE_ANIMATION_HPP
#include "tz/core/time.hpp"
#include "images.hpp"

namespace game
{
	struct AnimationInfo
	{
		std::vector<TextureID> frames;
		unsigned int fps;
		bool loop;
	};

	class Animation
	{
	public:
		Animation(AnimationInfo info);
		unsigned int length_millis() const;
		bool complete() const;
		const AnimationInfo& get_info() const;
		TextureID get_texture();
	private:
		AnimationInfo info;
		tz::Duration begin;
	};

	enum class AnimationID
	{
		PlayerClassic_Death,
		PlayerClassic_Idle,
		Count
	};

	Animation play_animation(AnimationID anim);
}

#endif // REDNIGHTMARE_ANIMATION_HPP
