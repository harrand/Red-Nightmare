#ifndef REDNIGHTMARE_ACTOR_HPP
#define REDNIGHTMARE_ACTOR_HPP
#include "animation.hpp"
#include "tz/core/containers/enum_field.hpp"

namespace game
{
	/// Describes immutable characteristics for a specific actor.
	enum class ActorFlag
	{
		/// Actor will be targetted by things that target players.
		Player,
		/// Move around with WASD.
		KeyboardControlled,
		/// Click-to-move to targetted location.
		MouseControlled,
		/// Computer controlled. Tries to chase and kill any nearby players.
		HostileGhost,
		/// Actor is never considered dead, even if its health is zero.
		Invincible
	};
	using ActorFlags = tz::EnumField<ActorFlag>;

	/// Describes internal actions being performed by an actor this fixed update.
	enum class ActorAction
	{
		/// Meta-action. Normally every fixed-update we clear out all previous actions. If this action exists though, any actions related to motion (e.g MoveLeft) are preserved a single time.
		SceneMessage_MaintainMotion,
		/// Actor sprite should be horizontally flipped until the next fixed update.
		HorizontalFlip,
		/// Actor should move to the left using its base movement speed.
		MoveLeft,
		/// Actor should move to the right using its base movement speed.
		MoveRight,
		/// Actor should move downwards using its base movement speed.
		MoveDown,
		/// Actor should move upwards using its base movement speed.
		MoveUp,
		/// Actor should chase the nearest player.
		ChasePlayer,
		/// Actor should move to the mouse cursor.
		FollowMouse,
		/// Actor can't do any action until its current animation finishes.
		AnimationPause,
	};
	using ActorActions = tz::EnumField<ActorAction>;

	/// Describes the general appearance of the actor. Will be used to automatically select which animations will be suitable for use. This might be refactored into something better.
	enum class ActorSkin
	{
		/// Actor looks like the player from the old incarnations of Red Nightmare.
		PlayerClassic,
		/// Actor looks like the Nightmare boss.
		Nightmare
	};

	enum class ActorType
	{
		/// A keyboard-controlled player using the classic skin.
		PlayerClassic,
		/// A computer-controlled player using the classic skin, but all it does is chase any nearby real players.
		PlayerClassic_TestEvil,
		/// A computer-controlled boss monster, called Nightmare that uses its own skin.
		Nightmare
	};

	struct Actor
	{
		ActorType type = {};
		/// Describes the characteristics of the actor.
		ActorFlags flags = {};
		/// Base movement speed of the actor.
		float base_movement = 0.0f;
		/// Base damage dealt to enemies every fixed update.
		float base_damage = 0.0083f;
		/// Maximum health of the actor.
		float max_health = 10;
		/// Current health of the actor.
		float current_health = 10;
		/// Describes appearance of the actor.
		ActorSkin skin = {};
		/// Describes what the actor is upto for this fixed-update.
		ActorActions actions = {};
		/// The animation currently playing on the actor.
		Animation animation = play_animation(AnimationID::Missing);

		void update();
		bool dead() const;
		void dbgui();
		static Actor null(){return {};}
		bool operator==(const Actor& rhs) const = default;
	private:
		/// Figure out which animation we should be using using all the actions at the end of the fixed-update.
		void evaluate_animation();
		/// Set the animation, but if we're already running that animation don't reset it.
		void assign_animation(AnimationID id);
		/// Assign animation, but block all actions until the animation completes.
		void assign_blocking_animation(AnimationID id);
		void refresh_actions();
	};

	Actor create_actor(ActorType type);
}

#endif // REDNIGHTMARE_ACTOR_HPP
