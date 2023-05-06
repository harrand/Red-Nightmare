#include "gamelib/gameplay/actor/combat_log.hpp"
#include <limits>

namespace rnlib
{
	combat_event combat_event::null()
	{
		return
		{
			.caster_uuid = std::numeric_limits<std::size_t>::max(),
			.target_uuid = std::numeric_limits<std::size_t>::max(),
			.value = 0
		};
	}

	std::span<const combat_event> combat_log::container() const
	{
		return this->log;
	}

	void combat_log::add(combat_event evt)
	{
		this->log.push_back(evt);
	}

	std::size_t combat_log::size() const
	{
		return this->log.size();
	}
}
