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

		bool operator==(const AnimationInfo& info) const = default;
	};

	class Animation
	{
	public:
		Animation(AnimationInfo info);
		unsigned int length_millis() const;
		bool complete() const;
		const AnimationInfo& get_info() const;
		TextureID get_texture();

		bool operator==(const Animation& rhs) const;
	private:
		AnimationInfo info;
		tz::Duration begin;
	};

	enum class AnimationID
	{
		Missing,
		PlayerClassic_Death,
		PlayerClassic_Idle,
		PlayerClassic_MoveDown,
		PlayerClassic_MoveUp,
		PlayerClassic_MoveSide,
		PlayerClassic_DefaultFireball_Idle,
		Nightmare_Spawn,
		Nightmare_Death,
		Nightmare_Idle,
		Nightmare_MoveDown,
		Nightmare_MoveUp,
		Nightmare_MoveSide,
		Count
	};

	Animation play_animation(AnimationID anim);
}

#endif // REDNIGHTMARE_ANIMATION_HPP
