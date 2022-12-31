#ifndef REDNIGHTMARE_FLAG_HPP
#define REDNIGHTMARE_FLAG_HPP
#include "entity.hpp"
#include "action.hpp"
#include "light.hpp"
#include "hdk/data/vector.hpp"
#include "hdk/memory/clone.hpp"
#include "tz/core/containers/enum_field.hpp"
#include "tz/core/peripherals/mouse.hpp"

namespace game
{
	struct Actor;
	enum class ActorType;
	enum class ActorSkin;

	enum class FlagID
	{
		Light,
		InvisibleWhileDead,
		BlockingAnimations,
		DoNotGarbageCollect,
		CustomGarbageCollectPeriod,
		/// Actor should be tracked in runtime stats.
		StatTracked,
		/// Actor represents some way of going 'down' into the next level.
		GoesDownALevel,
		/// Actor represents some way of going 'up' into an upper level.
		GoesUpALevel,
		/// Actor control is suppressed temporarily, no mouse/keyboard controls work for the time being.
		SuppressedControl,
		/// Actor is considered a player.
		Player,
		/// Actor is considered an obstacle.
		Obstacle,
		/// Actor is considered a projectile.
		Projectile,
		/// Actor has a custom scale.
		CustomScale,
		/// Actor has a custom reach distance.
		CustomReach,
		/// Actor will launch in a direction towards the mouse cursor.
		ClickToLaunch,
		/// Actor will attack nearby actors if the predicate is true. At present there is no aggro range - it always chases it and may run into walls doing so.
		AggressiveIf,
		/// Actor will not be chased by aggressive enemies, but it can still be harmed.
		Stealth,
		/// Actor cannot be harmed by aggressive enemies.
		Unhittable,
		/// Actor can be manually controlled by the keyboard controls.
		KeyboardControlled,
		/// Actor can be manually controlled, it continually chases the mouse cursor.
		MouseControlled,
		/// Actor can never die.
		Invincible,
		/// Whenever actor damages another, it also damages itself.
		SelfRecoil,
		/// Actor damages itself every fixed update.
		Rot,
		/// Actor respawns on click.
		RespawnOnClick,
		/// Actor respawns as soon as it dies.
		RespawnOnDeath,
		/// Actor performs an action whenever the user left-clicks their mouse.
		ActionOnClick,
		/// Actor performs an action when it dies. Note that some actions require the actor to be alive, which won't work here.
		ActionOnDeath,
		/// Actor performs an action when it hits something.
		ActionOnHit,
		/// Actor performs an action which something else hits it.
		ActionOnStruck,
		/// Actor performs an action when it touches an actor that satisfies a predicate.
		ActionOnActorTouch,
		/// Actor performs an action when it respawns.
		ActionOnRespawn,
		/// Actor performs an action when it leaves the game (view) boundaries.
		ActionOnOOB,
		/// Actor performs an action when it leaves the level boundaries.
		ActionOnOOL,
		/// Actor has one of a set of random skins instead of its default skin.
		RandomSkin,
		/// Actor will die as soon as its current animation ends. Note that this has no effect on looping animations.
		DieOnAnimationFinish,
		/// Actor will harm other colliding actors if a predicate is satisfied.
		HazardousIf,
		/// Actor will collide with certain other actors.
		Collide,
		/// Actor will periodically wander if it has nothing else to do.
		WanderIfIdle,
		/// Repeatedly perform a set of actions on a regular timer.
		ActionOnRepeat,
		/// Actor position is permanently attached to that of another, by UUID. If the target actor dies, the flag disappears.	
		AttachedToActor,
	};

	template<FlagID ID>
	struct FlagParams{};

	class IFlag : public hdk::unique_cloneable<IFlag>
	{
	public:
		virtual constexpr FlagID get_id() const = 0;
		virtual ~IFlag() = default;
	};

	template<FlagID ID>
	class Flag : public IFlag
	{
	public:
		Flag(FlagParams<ID> params = {}): params(std::move(params)){}
		[[nodiscard]] virtual std::unique_ptr<IFlag> unique_clone() const
		{
			return static_cast<std::unique_ptr<IFlag>>(std::make_unique<Flag<ID>>(*this));
		}
		virtual constexpr FlagID get_id() const override{return ID;}
		const FlagParams<ID>& data() const{return this->params;}
		FlagParams<ID>& data(){return this->params;}
	private:
		FlagParams<ID> params;
	private:
	};
	
	// Flag implementations.
	template<>
	struct FlagParams<FlagID::Light>
	{
		PointLight light;
		hdk::vec2 offset = hdk::vec2::zero();
		// How much should the power of the light change sinusoidally over time?
		float power_variance = 0.0f;
		// How quickly should the variance occur (1.0f means sin(time in seconds))
		float variance_rate = 1.0f;
		// What is the minimum % of original power that should be possible during variance?
		float min_variance_pct = 0.3f;
		// What is the maximum % of original power that should be possible during variance?
		float max_variance_pct = 1.5f;
		// If true, the power of the light is proportional to the % health of the actor. If false, no change.
		bool power_scale_with_health_pct = false;
		// Impl detail. Don't touch this.
		float time = 0.0f;
	};

	template<>
	struct FlagParams<FlagID::CustomGarbageCollectPeriod>
	{
		unsigned long long delay_millis = 75000ull;
	};

	template<>
	struct FlagParams<FlagID::CustomScale>
	{
		hdk::vec2 scale;
	};

	template<>
	struct FlagParams<FlagID::CustomReach>
	{
		float reach;
	};

	template<>
	struct FlagParams<FlagID::ClickToLaunch>
	{
		float internal_cooldown = 0.0f;
		unsigned long long launch_time = 0ull;
	};

	template<>
	struct FlagParams<FlagID::AggressiveIf>
	{
		std::function<bool(const Actor&, const Actor&)> predicate;
	};
	
	struct ActionClickData
	{
		ActionEntity actions;
		tz::Delay icd;
	};

	template<>
	struct FlagParams<FlagID::ActionOnClick>
	{
		std::unordered_map<tz::MouseButton, ActionClickData> action_map;
	};

	template<>
	struct FlagParams<FlagID::ActionOnDeath>
	{
		ActionEntity actions;
	};

	template<>
	struct FlagParams<FlagID::ActionOnHit>
	{
		ActionEntity actions;
		ActionEntity hittee_actions = {};
	};

	template<>
	struct FlagParams<FlagID::ActionOnStruck>
	{
		ActionEntity actions;
		ActionEntity striker_actions = {};
		float icd = 0.0f;
		float internal_cooldown = 0.0f;
	};


	template<>
	struct FlagParams<FlagID::ActionOnActorTouch>
	{
		std::function<bool(const Actor&, const Actor&)> predicate;
		ActionEntity actions;
		ActionEntity touchee_actions;
		bool allow_dead = false;
	};

	template<>
	struct FlagParams<FlagID::ActionOnRespawn>
	{
		ActionEntity actions;
	};

	template<>
	struct FlagParams<FlagID::ActionOnOOB>
	{
		ActionEntity actions;
	};

	template<>
	struct FlagParams<FlagID::ActionOnOOL>
	{
		ActionEntity actions;
	};

	template<>
	struct FlagParams<FlagID::RandomSkin>
	{
		std::vector<ActorSkin> skins;
	};

	template<>
	struct FlagParams<FlagID::HazardousIf>
	{
		std::function<bool(const Actor&, const Actor&)> predicate;
	};

	template<>
	struct FlagParams<FlagID::Collide>
	{
		tz::EnumField<ActorType> collision_filter = {};
		/// If set, anything that does not satisfy this predicate is excluded, unless whitelisted.
		std::function<bool(const Actor&)> filter_predicate = nullptr;
		tz::EnumField<ActorType> collision_blacklist = {};
		/// If set, anything that satisfies this predicate is excluded, unless whitelisted.
		std::function<bool(const Actor&)> blacklist_predicate = nullptr;
	};

	template<>
	struct FlagParams<FlagID::WanderIfIdle>
	{
		float min_wander_range = 0.2f;
		float max_wander_range = 0.3f;
		float wander_chance = 0.001f;
	};

	template<>
	struct FlagParams<FlagID::ActionOnRepeat>
	{
		float period;
		float current_time = 1000.0f;
		std::function<bool(const Actor&)> predicate;
		ActionEntity actions;
	};

	template<>
	struct FlagParams<FlagID::AttachedToActor>
	{
		std::size_t actor_uuid;
	};

	class FlagEntity : public Entity<FlagID, IFlag, Flag, FlagParams>
	{
		using Entity<FlagID, IFlag, Flag, FlagParams>::Entity;
	};

	template<>
	struct ActionParams<ActionID::ApplyFlag>
	{
		FlagEntity flags;
	};
}

#endif // REDNIGHTMARE_FLAG_HPP
