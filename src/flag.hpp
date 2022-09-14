#ifndef REDNIGHTMARE_FLAG_HPP
#define REDNIGHTMARE_FLAG_HPP
#include "entity.hpp"
#include "tz/core/vector.hpp"

namespace game
{
	enum class ActorType;
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
		/// When the actor dies, spawns another actor.
		SpawnOnDeath,
		/// Actor can be manually controlled by the keyboard controls.
		KeyboardControlled,
		/// Actor can be manually controlled, it continually chases the mouse cursor.
		MouseControlled
	};

	template<FlagID ID>
	struct FlagParams{};

	class IFlag
	{
	public:
		virtual constexpr FlagID get_id() const = 0;
	};

	template<FlagID ID>
	class Flag : public IFlag
	{
	public:
		Flag(FlagParams<ID> params = {}): params(params){}
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
	struct FlagParams<FlagID::SpawnOnDeath>
	{
		tz::EnumField<ActorType> types;
	};

	// Represents something that stores flags.
	class FlagEntity : public Entity<FlagID, IFlag, Flag, FlagParams>
	{
		using Entity<FlagID, IFlag, Flag, FlagParams>::Entity;
	};
}

#endif // REDNIGHTMARE_FLAG_HPP
