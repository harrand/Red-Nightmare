#include "action.hpp"
#include "hdk/profile.hpp"

namespace game
{
	void ActionEntity::update()
	{
		HDK_PROFZONE("ActionEntity - Update", 0xFF00AA00);
		// Remove all completed actions.
		this->components.erase(std::remove_if(this->components.begin(), this->components.end(), [](const auto& comp_ptr)
		{
			return comp_ptr->get_is_complete();
		}), this->components.end());
	}
}
