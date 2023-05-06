#include "gamelib/gameplay/actor/spell/spell.hpp"
#include "gamelib/gameplay/actor/system.hpp"
#include "tz/core/debug.hpp"

namespace rnlib
{
	spell create_spell(spell_id id)
	{
		spell sp = [](spell_id i)->spell
		{
			switch(i)
			{
				case spell_id::heal:
				{
					#include "gamelib/gameplay/actor/spell/types/heal.spell"
				}
				break;
				case spell_id::hand_of_death:
				{
					#include "gamelib/gameplay/actor/spell/types/hand_of_death.spell"
				}
				break;
			}
			tz::error("Attempted to create spell of an unknown type. Add support for this spell type.");
			return {};
		}(id);
		sp.id = id;
		return sp;
	}
}
