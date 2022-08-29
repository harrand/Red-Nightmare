#ifndef REDNIGHTMARE_ACTOR_HPP
#define REDNIGHTMARE_ACTOR_HPP
#include "animation.hpp"
#include "tz/core/containers/enum_field.hpp"

namespace game
{
	enum class ActorFlag
	{
		KeyboardControlled,
	};
	using ActorFlags = tz::EnumField<ActorFlag>;

	enum class ActorAction
	{
		HorizontalFlip
	};
	using ActorActions = tz::EnumField<ActorAction>;

	enum class ActorSkin
	{
		PlayerClassic
	};

	enum class ActorType
	{
		PlayerClassic
	};

	struct Actor
	{
		ActorFlags flags = {};
		float base_movement = 0.0f;
		ActorSkin skin = {};
		ActorActions actions = {};
		Animation animation = play_animation(static_cast<AnimationID>(0));

		void update();
		static Actor null(){return {};}
		bool operator==(const Actor& rhs) const = default;
	private:
		void assign_animation(AnimationID id);
	};

	Actor create_actor(ActorType type);
}

#endif // REDNIGHTMARE_ACTOR_HPP
