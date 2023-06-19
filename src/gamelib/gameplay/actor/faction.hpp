#ifndef RNLIB_GAMEPLAY_ACTOR_faction_t_HPP
#define RNLIB_GAMEPLAY_ACTOR_faction_t_HPP
#include "tz/core/data/enum_field.hpp"
#include "tz/core/data/vector.hpp"
#include <array>

namespace rnlib
{
	enum class faction_t
	{
		pyr,
		anthor,
		white_legion,
		pure_evil, // meta faction. enemy to everyone (excluding pure evil).
		pure_good, // meta faction. friend to everyone (excluding pure evil).
		_count
	};
	// to add a new faction:
	// - add the enum value (above)
	// - give it a name (below)
	// - give it a colour (below)
	// - populate its table of allegiences (top of factioncpp)
	// - DO NOT RE-ORDER THE FACTIONS, ADD TO THE BOTTOM OF THE LIST. 
	// do all these, or the behaviour will be undefined.	

	constexpr std::array<const char*, (int)faction_t::_count> faction_names
	{
		"pyr", "anthor", "white legion", "pure evil", "pure good"
	};

	constexpr std::array<tz::vec3, (int)faction_t::_count> faction_colours
	{
		tz::vec3{1.0f, 0.5f, 0.0f}, // pyr,
		tz::vec3{102.0f/255.0f, 0.0f, 204.0f/255.0f}, // anthor
		tz::vec3{1.0f, 1.0f, 153.0f/255.0f}, // white legion
		tz::vec3{0.5f, 0.0f, 0.0f}, // pure evil
		tz::vec3{0.0f, 0.5f, 0.0f}, // pure good
	};

	using factions_t = tz::enum_field<faction_t>;

	enum class allegience_t
	{
		neutral,
		friendly,
		enemy
	};

	allegience_t get_allegience(faction_t lhs, faction_t rhs);
	allegience_t get_allegience(faction_t lhs, factions_t rhss);
	allegience_t get_allegience(factions_t lhss, faction_t rhs);
	allegience_t get_allegience(factions_t lhss, factions_t rhss);
}

#endif // RNLIB_GAMEPLAY_ACTOR_faction_t_HPP
