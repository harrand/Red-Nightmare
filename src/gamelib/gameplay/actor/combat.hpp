#ifndef RNLIB_GAMEPLAY_ACTOR_COMBAT_HPP
#define RNLIB_GAMEPLAY_ACTOR_COMBAT_HPP
#include <cstddef>

namespace rnlib
{
	struct actor;
	enum class spell_id;
	class combat_log;
	namespace combat
	{
		void damage(actor& target, actor* attacker, spell_id cause, std::size_t amt);
		void heal(actor& target, actor* healer, spell_id cause, std::size_t amt);
		const combat_log& get_log();	
	}
}

#endif // RNLIB_GAMEPLAY_ACTOR_COMBAT_HPP
