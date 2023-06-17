#include "gamelib/gameplay/actor/faction.hpp"

namespace rnlib
{
	using allegiences = std::array<allegience_t, (int)faction_t::_count>;
	#define fr allegience_t::friendly
	#define ne allegience_t::neutral
	#define en allegience_t::enemy
	std::array<allegiences, (int)faction_t::_count> impl_get_allegience_table()
	{
		return
		{
			
			//                              pyr        ,anthor     ,white_legion
			/* pyr           */ allegiences{fr         ,en         ,ne},
			/* anthor        */ allegiences{en         ,fr         ,en},
			/* white legion  */ allegiences{ne         ,en         ,fr},
		};
	}

	allegience_t get_allegience(faction_t lhs, faction_t rhs)
	{
		return impl_get_allegience_table()[(int)lhs][(int)rhs];
	}

	allegience_t get_allegience(faction_t lhs, factions_t rhss)
	{
		allegience_t ret = allegience_t::neutral;
		for(faction_t f : rhss)
		{
			switch(get_allegience(lhs, f))
			{
				case allegience_t::friendly:
					if(ret != allegience_t::enemy)
					{
						ret = allegience_t::friendly;
					}
				break;
				case allegience_t::enemy:
					ret = allegience_t::enemy;
				break;
				default: break;
			}
		}
		return ret;
	}

	allegience_t get_allegience(factions_t lhss, faction_t rhs)
	{
		return get_allegience(rhs, lhss);
	}

	allegience_t get_allegience(factions_t lhss, factions_t rhss)
	{
		allegience_t ret = allegience_t::neutral;
		for(faction_t f : lhss)
		{
			switch(get_allegience(f, rhss))
			{
				case allegience_t::friendly:
					if(ret != allegience_t::enemy)
					{
						ret = allegience_t::friendly;
					}
				break;
				case allegience_t::enemy:
					ret = allegience_t::enemy;
				break;
				default: break;
			}
		}
		return ret;
	}
}
