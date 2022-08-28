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

	enum class ActorType
	{
		Player
	};

	struct Actor
	{
		ActorFlags flags = {};
		float base_movement = 0.0f;
		Animation animation = play_animation(static_cast<AnimationID>(0));

		static Actor null(){return {};}
		bool operator==(const Actor& rhs) const = default;
	};

	Actor create_actor(ActorType type);
}

#endif // REDNIGHTMARE_ACTOR_HPP
