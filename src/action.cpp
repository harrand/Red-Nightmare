#include "action.hpp"

namespace game
{
	std::size_t ActionEntity::size() const
	{
		return this->actions.size();
	}

	void ActionEntity::update()
	{
		// Remove all completed actions.
		this->actions.erase(std::remove_if(this->actions.begin(), this->actions.end(), [](const std::unique_ptr<IAction>& action_ptr)
		{
			return action_ptr->get_is_complete();
		}), this->actions.end());
	}
}
