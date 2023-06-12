#ifndef RNLIB_GAMEPLAY_ACTOR_COMBAT_HPP
#define RNLIB_GAMEPLAY_ACTOR_COMBAT_HPP
#include "tz/core/data/enum_field.hpp"
#include <cstddef>

namespace rnlib
{
	struct actor;
	enum class spell_id;
	class combat_log;
	enum class combat_damage_type;
	using combat_damage_types = tz::enum_field<combat_damage_type>;
	
	namespace combat
	{
		const char* get_damage_type_name(const combat_damage_types& type);
		void damage(actor& target, actor* attacker, spell_id cause, std::size_t amt, const combat_damage_types& type);
		void heal(actor& target, actor* healer, spell_id cause, std::size_t amt, const combat_damage_types& type);
		const combat_log& get_log();	
	}
}

#endif // RNLIB_GAMEPLAY_ACTOR_COMBAT_HPP
