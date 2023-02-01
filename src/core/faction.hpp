#ifndef REDNIGHTMARE_FACTION_HPP
#define REDNIGHTMARE_FACTION_HPP

namespace game
{
	/// Actors are members of zero or more factions. Factions determine who the actor is friendly, neutral or hostile to.
	enum class Faction
	{
		/// Default faction. Neutral to everyone.
		Default,
		/// Actor is friendly to players.
		PlayerFriend,
		/// Actor is enemy to players.
		PlayerEnemy,
		/// Actor is friendly to everyone.
		PureFriend,
		/// Actor is enemy to everyone.
		PureEnemy,
	};
}

#endif // REDNIGHTMARE_FACTION_HPP
