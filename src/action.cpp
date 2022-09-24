#include "action.hpp"
#include "tz/core/profiling/zone.hpp"

namespace game
{
	void ActionEntity::update()
	{
		TZ_PROFZONE("ActionEntity - Update", TZ_PROFCOL_GREEN);
		// Remove all completed actions.
		this->components.erase(std::remove_if(this->components.begin(), this->components.end(), [](const auto& comp_ptr)
		{
			return comp_ptr->get_is_complete();
		}), this->components.end());
	}
}
