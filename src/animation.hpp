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
		tz::duration begin;
	};

	enum class AnimationID
	{
		Missing,
		Invisible,
		PlayerAkhara_Cast,
		PlayerAkhara_Death,
		PlayerAkhara_MoveDown,
		PlayerAkhara_EndCast,
		PlayerAkhara_Idle,
		PlayerAkhara_MoveSide,
		PlayerAkhara_MoveUp,
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
		PlayerAkhara_DefaultFireball_Idle,
		PlayerAkhara_FireSmoke,
		BlockBreak,
		BloodSplatter,
		PlayerAkhara_FireExplosion,
		Nightmare_Spawn,
		Nightmare_Death,
		Nightmare_Idle,
		Nightmare_MoveDown,
		Nightmare_MoveUp,
		Nightmare_MoveSide,
		Material_Stone,
		Gui_Healthbar_Empty,
		Scenery_Gravestone_0,
		Scenery_Gravestone_1,
		Interactable_Torch_Off,
		Downwards_Trapdoor,
		Interactable_Stone_Stairs_Down_NX,
		Interactable_Stone_Stairs_Up_NX,
		Interactable_Stone_Stairs_Down_PY,
		Interactable_Stone_Stairs_Up_PY,
		Powerup_Sprint,
		Count
	};

	Animation play_animation(AnimationID anim);
}

#endif // REDNIGHTMARE_ANIMATION_HPP
