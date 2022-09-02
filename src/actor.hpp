#ifndef REDNIGHTMARE_ACTOR_HPP
#define REDNIGHTMARE_ACTOR_HPP
#include "animation.hpp"
#include "faction.hpp"
#include "action.hpp"
#include "stats.hpp"
#include "tz/core/containers/enum_field.hpp"

namespace game
{
	/// Describes immutable characteristics for a specific actor.
	enum class ActorFlag
	{
		/// Actor will be targetted by things that target players.
		Player,
		/// Actor will attack enemy targets without provocation.
		Aggressive,
		/// Move around with WASD.
		KeyboardControlled,
		/// Actor will always chase the mouse cursor.
		MouseControlled,
		// Actor will move towards the mouse cursor until it reaches it once.
		ChaseMouse,
		/// Actor is never considered dead, even if its health is zero.
		Invincible,
		/// Whenever actor causes damage, also hurts itself.
		SelfHarm,
		/// Actor constantly damages itself.
		Rot,
		/// If the mouse is left-clicked. Respawn.
		RespawnOnClick,
		/// Whenever actor dies, respawn in the same position.
		RespawnOnDeath,
		/// Whenever the actor is dead and touched by a player, respawn.
		DeadRespawnOnPlayerTouch,
		/// Whenever the actor is dead and touched by a player, resurrect.
		DeadResurrectOnPlayerTouch,
		/// When respawned, do so in a completely random location.
		RandomRespawnLocation,
		/// When respawned, do so at a player's location.
		RespawnOnPlayer,
		/// Attempts to deal damage to all actors nearby.
		HazardousToAll,
		/// Attempts to deal damage to any enemies that are nearby.
		HazardousToEnemies,
		/// Actor will very quickly charge to the mouse location upon left click. Note: Actors with ClickToLaunch ignore death mechanics.
		ClickToLaunch,
		/// If the actor ever stops, it instantly dies.
		DieAtRest,
		/// If the actor reaches out-of-bounds, respawn.
		RespawnIfOOB,
		/// If the actor reaches out-of-bounds, die.
		DieIfOOB,
		/// If the actor is dead, it is invisible.
		InvisibleWhileDead,
		/// If the actor dies, spawns a ghost.
		Haunted
	};
	using ActorFlags = tz::EnumField<ActorFlag>;

	/// Describes internal actions being performed by an actor this fixed update.
	enum class ActorAction
	{
		/// Meta-action. Normally every fixed-update we clear out all previous actions. If this action exists though, any actions related to motion (e.g MoveLeft) are preserved a single time.
		SceneMessage_MaintainMotion,
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
		/// On debug builds, looks like the missing placeholder texture. On non-debug builds, completely invisible.
		DebugOnlyVisible,
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
		Nightmare,
		/// An uninteractable actor which periodically spawns evil players.
		EvilPlayer_TestSpawner
	};

	struct Actor
	{
		ActorType type = {};
		/// Describes the characteristics of the actor.
		ActorFlags flags = {};
		/// Describes the allegiances of the actor.
		Faction faction = Faction::Default;
		/// Current stats of the actor.
		Stats base_stats = {};
		/// Describes appearance of the actor.
		ActorSkin skin = {};
		/// Describes what the actor is upto for this fixed-update.
		ActorActions actions = {};
		// New actions implementation: Describes what the actor is upto for this fixed-update.
		ActionEntity entity = {};
		/// The animation currently playing on the actor.
		Animation animation = play_animation(AnimationID::Missing);
		/// All buffs to stats currently applied on the actor.
		StatBuffs buffs = {};

		void update();
		bool dead() const;
		void dbgui();
		void damage(Actor& victim);
		void respawn();
		Stats get_current_stats() const;

		bool is_ally_of(const Actor& actor) const;
		bool is_enemy_of(const Actor& actor) const;

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
