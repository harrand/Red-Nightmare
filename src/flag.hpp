#ifndef REDNIGHTMARE_FLAG_HPP
#define REDNIGHTMARE_FLAG_HPP
#include "entity.hpp"
#include "action.hpp"
#include "tz/core/vector.hpp"
#include "tz/core/interfaces/cloneable.hpp"
#include "tz/core/containers/enum_field.hpp"

namespace game
{
	struct Actor;
	enum class ActorType;
	enum class ActorSkin;

	enum class FlagID
	{
		/// Actor is considered a player.
		Player,
		/// Actor has a custom scale.
		CustomScale,
		/// Actor has a custom reach distance.
		CustomReach,
		/// Actor will launch in a direction towards the mouse cursor.
		ClickToLaunch,
		/// Actor will attack nearby enemy targets. At present there is no aggro range - it always chases it and may run into walls doing so.
		Aggressive,
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
		/// Actor performs an action when it dies. Note that some actions require the actor to be alive, which won't work here.
		ActionOnDeath,
		/// Actor performs an action when it hits something.
		ActionOnHit,
		/// Actor performs an action which something else hits it.
		ActionOnStruck,
		/// Actor performs an action when it touches a living player.
		ActionOnPlayerTouch,
		/// Actor performs an action when it respawns.
		ActionOnRespawn,
		/// Actor performs an action when it leaves the game boundaries.
		ActionOnOOB,
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
	};

	template<FlagID ID>
	struct FlagParams{};

	class IFlag : public tz::IUniqueCloneable<IFlag>
	{
	public:
		virtual constexpr FlagID get_id() const = 0;
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
	struct FlagParams<FlagID::CustomScale>
	{
		tz::Vec2 scale;
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
	struct FlagParams<FlagID::ActionOnDeath>
	{
		ActionEntity actions;
	};

	template<>
	struct FlagParams<FlagID::ActionOnHit>
	{
		ActionEntity actions;
	};

	template<>
	struct FlagParams<FlagID::ActionOnStruck>
	{
		ActionEntity actions;
		float icd = 0.0f;
		float internal_cooldown = 0.0f;
	};


	template<>
	struct FlagParams<FlagID::ActionOnPlayerTouch>
	{
		ActionEntity actions;
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
		tz::EnumField<ActorType> collision_blacklist = {};
	};

	template<>
	struct FlagParams<FlagID::WanderIfIdle>
	{
		float min_wander_range = 0.2f;
		float max_wander_range = 0.3f;
		float wander_chance = 0.001f;
	};

	class FlagEntity : public Entity<FlagID, IFlag, Flag, FlagParams>
	{
		using Entity<FlagID, IFlag, Flag, FlagParams>::Entity;
	};
}

#endif // REDNIGHTMARE_FLAG_HPP
