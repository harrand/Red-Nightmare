#ifndef REDNIGHTMARE_ACTOR_HPP
#define REDNIGHTMARE_ACTOR_HPP
#include "animation.hpp"
#include "faction.hpp"
#include "flag.hpp"
#include "action.hpp"
#include "stats.hpp"
#include "tz/core/containers/enum_field.hpp"

namespace game
{

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
		/// Actor looks like the redesigned Akhara Ceitus low-resolution sprites.
		PlayerAkhara_LowPoly,
		/// Actor looks like a zombified version of Akhara. One of the ghost skins.
		GhostZombie,
		/// Actor looks like a zombified Michael Jackson from thriller ;)
		GhostMJZombie,
		/// Actor looks like a banshee.
		GhostBanshee,
		/// Actor looks like a fiery-smokey particle effect.
		FireSmoke,
		/// Actor looks like a fiery explosion particle effect.
		FireExplosion,
		/// Actor looks like the Nightmare boss.
		Nightmare,
		/// Actor looks like Akhara's default fireball.
		PlayerAkhara_DefaultFireball,
		/// Actor looks like a plain stone texture.
		Material_Stone,
		/// Actor looks like a health bar.
		Gui_Healthbar,
		/// Actor looks like a gravestone (type 0, cross).
		Scenery_Gravestone_0,
		/// Actor looks like a gravestone (type 1, block).
		Scenery_Gravestone_1,
		/// Actor looks like some stone stairs going downwards, facing the positive-y direction.
		Interactable_Stone_Stairs_Down_PY,
		/// Actor looks like some stone stairs going upwards, facing the positive-y direction.
		Interactable_Stone_Stairs_Up_PY,
		/// Actor looks like a blood splatter.
		BloodSplatter,
		/// Actor looks like a sprint powerup.
		Powerup_Sprint,
		/// On debug builds, looks like the missing placeholder texture. On non-debug builds, completely invisible.
		DebugOnlyVisible,
		/// Actor is invisible.
		Invisible,
	};

	enum class ActorType
	{
		/// A keyboard-controlled player using the classic skin.
		PlayerAkhara,
		/// A computer-controlled ghost zombie enemy.
		GhostZombie,
		/// A computer-controlled ghost banshee enemy. It's slower than other zombies, but can chill enemy players, slowing their movement.
		GhostBanshee,
		/// When a GhostBanshee dies, it will become a spirit which searches for a nearby zombie to possess. If it does, it turns the zombie into another banshee.
		GhostBanshee_Spirit,
		/// An actor which acts like a fireball projectile, shooting in a random direction.
		ChaoticFireball,
		/// An actor which acts as a fiery particle effect. Dies by itself after a while.
		FireSmoke,
		/// An actor which acts as a fiery explosion particle effect. Dies by itself after a while, but deals large damage to enemies in its radius.
		FireExplosion,
		/// An actor which displays an actor spurting blood after taking damage. Has no other iteration.
		BloodSplatter,
		/// A computer-controlled boss monster, called Nightmare that uses its own skin.
		Nightmare,
		/// An uninteractable actor which periodically spawns evil players.
		GhostZombie_Spawner,
		/// An uninteractable actor which periodically spawns fireballs launching towards a player.
		Fireball_Spawner,

		/// A visible wall, blocking other actors' paths.
		Wall,
		/// An invisible wall, blocking other actors' paths.
		InvisibleWall,
		/// An invisible wall, blocking other actor's paths. Projectiles, however, can pass through without a problem.
		InvisibleWallProjectilePass,

		/// Just like a wall, but is destructible by the player.
		WallDestructible,
		/// A breakable gravestone, good for single-use cover.
		Scenery_Gravestone,
		/// Some stone stairs going downwards, facing the positive-y direction. Player will go to the next level by going down them.
		Interactable_Stone_Stairs_Down_PY,
		/// Some stone stairs going upwards, facing the positive-y direction. Player will go to the previous level by going up them.
		Interactable_Stone_Stairs_Up_PY,
		/// A powerup that is gives the player sprint on consumption.
		CollectablePowerup_Sprint,
		/// An invisible actor that is designed to sit on a road. The player should receive a speed boost from this actor when on a road.
		RoadSpeedBoost,
		/// Meta-actor. Represents 'the world'. e.g if you take damage from rot, or an environmental effect, the attacker will be the world.
		World,
		Count
	};

	using ActorTypes = tz::EnumField<ActorType>;

	constexpr unsigned short default_layer = std::numeric_limits<unsigned short>::max() / 2;

	struct Actor
	{
		static std::size_t uuid_count;
		static constexpr std::size_t NullID = std::numeric_limits<std::size_t>::max();
		ActorType type = {};
		/// Describes the characteristics of the actor.
		FlagEntity flags = {};
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
		/// Colour representing the actor in a level image. If this is black, then the actor cannot be represented in a level.
		hdk::vec3ui palette_colour = {0u, 0u, 0u};
		/// Name for the entity.
		const char* name = "<Unnamed Entity>";
		/// Affects how much other actors are pushed if this actor has ActorFlag::Collide.
		float density = 1.0f;
		/// Duration at last update.
		tz::Duration last_update = tz::system_time();
		Actor* target = nullptr;
		unsigned short layer = default_layer;
		std::size_t uuid = uuid_count++;

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
		void evaluate_buffs();
		/// Figure out which animation we should be using using all the actions at the end of the fixed-update.
		void evaluate_animation();
		/// Set the animation, but if we're already running that animation don't reset it.
		void assign_animation(AnimationID id);
	};

	Actor create_actor(ActorType type);
}

#endif // REDNIGHTMARE_ACTOR_HPP
