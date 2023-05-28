#include "gamelib/gameplay/actor/spell/buff.hpp"

namespace rnlib
{
	buff create_buff(buff_id id)
	{
		buff b = [id]()->buff
		{
			switch(id)
			{
			}
			return {};
		}();
		b.id = id;
		return b;
	}
}
