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
		/// If the actor dies, a fiery explosion happens (Blessing of Volatile Fire).
		ExplodeOnDeath,
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
		Haunted,
		/// If the actor is doing a non-looping animation, they won't take any action until it is complete.
		BlockingAnimations,
		/// If an actor is dead for a long time, the Scene will automatically delete it. If an actor has this flag, this will never happen.
		DoNotGarbageCollect,
	};
	using ActorFlags = tz::EnumField<ActorFlag>;

	enum class ActorMotion
	{
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown
	};
	using ActorMotions = tz::EnumField<ActorMotion>;

	/// Describes the general appearance of the actor. Will be used to automatically select which animations will be suitable for use. This might be refactored into something better.
	enum class ActorSkin
	{
		/// Actor looks like the player from the old incarnations of Red Nightmare.
		PlayerClassic,
		/// Actor looks like the redesigned Akhara Ceitus low-resolution sprites.
		PlayerClassic_LowPoly,
		/// Actor looks like a fiery-smokey particle effect.
		FireSmoke,
		/// Actor looks like a fiery explosion particle effect.
		FireExplosion,
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
		/// An actor which acts as a fiery particle effect. Dies by itself after a while.
		FireSmoke,
		/// An actor which acts as a fiery explosion particle effect. Dies by itself after a while, but deals large damage to enemies in its radius.
		FireExplosion,
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
		/// Describes the actors movements for this fixed-update.
		ActorMotions motion = {};
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
	};

	Actor create_actor(ActorType type);
}

#endif // REDNIGHTMARE_ACTOR_HPP
