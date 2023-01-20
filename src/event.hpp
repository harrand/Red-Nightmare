#ifndef REDNIGHTMARE_EVENT_HPP
#define REDNIGHTMARE_EVENT_HPP
#include "tz/core/callback.hpp"
#include "actor.hpp"

namespace game
{
	struct ActorHitEvent
	{
		Actor& attacker;
		Actor& attackee;
	};

	struct ActorStruckEvent
	{
		Actor& attackee;
		Actor& attacker;
	};

	struct ActorKillEvent
	{
		Actor& killer;
		Actor& killee;
	};

	struct ActorDeathEvent
	{
		Actor& killee;
		Actor& killer;
	};

	struct ActorEventHandler
	{
	public:
		ActorEventHandler() = default;

		tz::callback<ActorHitEvent> actor_hit;
		tz::callback<ActorStruckEvent> actor_struck;
		tz::callback<ActorKillEvent> actor_kill;
		tz::callback<ActorDeathEvent> actor_death;
	};
}

#endif //REDNIGHTMARE_EVENT_HPP
