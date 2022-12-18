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

		void set_fps_multiplyer(float mul);
		float get_fps_multiplyer() const;

		bool operator==(const Animation& rhs) const;
	private:
		AnimationInfo info;
		float fps_multiplier = 1.0f;
		tz::Duration begin;
	};

	enum class AnimationID
	{
		Missing,
		Invisible,
		PlayerClassic_Death,
		PlayerClassic_Idle,
		PlayerClassic_MoveDown,
		PlayerClassic_MoveUp,
		PlayerClassic_MoveSide,
		PlayerClassic_LowPoly_Cast,
		PlayerClassic_LowPoly_Death,
		PlayerClassic_LowPoly_MoveDown,
		PlayerClassic_LowPoly_EndCast,
		PlayerClassic_LowPoly_Idle,
		PlayerClassic_LowPoly_MoveSide,
		PlayerClassic_LowPoly_MoveUp,
		GhostZombie_Death,
		GhostZombie_MoveDown,
		GhostZombie_Idle,
		GhostZombie_MoveSide,
		GhostZombie_MoveUp,
		GhostMJZombie_Death,
		GhostMJZombie_MoveDown,
		GhostMJZombie_Idle,
		GhostMJZombie_MoveSide,
		GhostMJZombie_MoveUp,
		GhostBanshee_Idle,
		GhostBanshee_Death,
		PlayerClassic_DefaultFireball_Idle,
		PlayerClassic_FireSmoke,
		BlockBreak,
		BloodSplatter,
		PlayerClassic_FireExplosion,
		Nightmare_Spawn,
		Nightmare_Death,
		Nightmare_Idle,
		Nightmare_MoveDown,
		Nightmare_MoveUp,
		Nightmare_MoveSide,
		Material_Stone,
		Powerup_Sprint,
		Count
	};

	Animation play_animation(AnimationID anim);
}

#endif // REDNIGHTMARE_ANIMATION_HPP
