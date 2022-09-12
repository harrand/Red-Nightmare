#include "action.hpp"

namespace game
{
	void ActionEntity::update()
	{
		// Remove all completed actions.
		this->components.erase(std::remove_if(this->components.begin(), this->components.end(), [](const auto& comp_ptr)
		{
			return comp_ptr->get_is_complete();
		}), this->components.end());
	}
}
