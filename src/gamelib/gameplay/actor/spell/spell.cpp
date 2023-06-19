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
				case spell_id::fiery_burst:
				{
					#include "gamelib/gameplay/actor/spell/types/fiery_burst.spell"
				}
				break;
				case spell_id::fireball:
				{
					#include "gamelib/gameplay/actor/spell/types/fireball.spell"
				}
				break;
				case spell_id::scorch:
				{
					#include "gamelib/gameplay/actor/spell/types/scorch.spell"
				}
				break;
				case spell_id::summon_skeleton:
				{
					#include "gamelib/gameplay/actor/spell/types/summon_skeleton.spell"
				}
				break;
				case spell_id::teleport:
				{
					#include "gamelib/gameplay/actor/spell/types/teleport.spell"
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
