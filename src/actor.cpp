#include "actor.hpp"

namespace game
{
	Actor create_actor(ActorType type)
	{
		switch(type)
		{
			case ActorType::Player:
				return
				{
					.flags = {ActorFlag::KeyboardControlled},
					.base_movement = 0.1f,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle)
				};
			break;
		}
		return Actor::null();
	}
}
