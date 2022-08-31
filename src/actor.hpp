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
		/// Actor will always chase the mouse cursor.
		MouseControlled,
		// Actor will move towards the mouse cursor until it reaches it once.
		ChaseMouse,
		/// Computer controlled. Tries to chase and kill any nearby players.
		HostileGhost,
		/// Actor is considered an ally to the player.
		Ally,
		/// Actor is never considered dead, even if its health is zero.
		Invincible,
		/// Whenever actor causes damage, also hurts itself.
		SelfHarm,
		/// Whenever actor dies, respawn in the same position.
		RespawnOnDeath,
		/// When respawned, do so in a completely random location.
		RandomRespawnLocation,
		/// Attempts to deal damage to all actors nearby. TODO: Implement.
		Hazardous,
		/// Actor will very quickly charge to the mouse location upon left click. Note: Actors with ClickToLaunch ignore death mechanics.
		ClickToLaunch,
		/// Actor will move 4x faster than normal until it stops.
		FastUntilRest,
		/// If the actor ever stops, it instantly dies.
		DieAtRest,
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
		/// Actor should move towards the mouse cursor this fixed update.
		FollowMouse,
		/// Actor can't do any action until its current animation finishes.
		AnimationPause,
		/// Actor should teleport to a random location in the world.
		RandomTeleport,
	};
	using ActorActions = tz::EnumField<ActorAction>;

	/// Describes the general appearance of the actor. Will be used to automatically select which animations will be suitable for use. This might be refactored into something better.
	enum class ActorSkin
	{
		/// Actor looks like the player from the old incarnations of Red Nightmare.
		PlayerClassic,
		/// Actor looks like the Nightmare boss.
		Nightmare,
		/// Actor looks like Akhara's default fireball.
		PlayerClassic_DefaultFireball,
	};

	enum class ActorType
	{
		/// A keyboard-controlled player using the classic skin.
		PlayerClassic,
		/// A computer-controlled player using the classic skin, but all it does is chase any nearby real players.
		PlayerClassic_TestEvil,
		/// An actor which acts as Akhara's passive elemental orb.
		PlayerClassic_Orb,
		/// A computer-controlled boss monster, called Nightmare that uses its own skin.
		Nightmare
	};
	
	constexpr float default_base_movement = 0.0f;
	constexpr float default_base_damage = 0.0083f;
	constexpr float default_max_health = 10;

	struct Actor
	{
		ActorType type = {};
		/// Describes the characteristics of the actor.
		ActorFlags flags = {};
		/// Base movement speed of the actor.
		float base_movement = default_base_movement;
		/// Base damage dealt to enemies every fixed update.
		float base_damage = default_base_damage;
		/// Maximum health of the actor.
		float max_health = default_max_health;
		/// Current health of the actor.
		float current_health = default_max_health;
		/// Describes appearance of the actor.
		ActorSkin skin = {};
		/// Describes what the actor is upto for this fixed-update.
		ActorActions actions = {};
		/// The animation currently playing on the actor.
		Animation animation = play_animation(AnimationID::Missing);

		void update();
		bool dead() const;
		void dbgui();
		void damage(Actor& victim);
		void respawn();
		static Actor null(){return {};}
		bool operator==(const Actor& rhs) const = default;
	private:
		bool is_ally_of(const Actor& actor) const;
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
