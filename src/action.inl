namespace game
{
	template<ActionID ID>
	ActionEntity::Handle ActionEntity::add(ActionParams<ID> params)
	{
		auto derived_ptr = std::make_unique<Action<ID>>(params);
		Action<ID>* derived_owner = derived_ptr.release();
		this->actions.push_back(std::unique_ptr<IAction>(static_cast<IAction*>(derived_owner)));
		return static_cast<tz::HandleValue>(this->actions.size());
	}

	template<ActionID ID>
	bool ActionEntity::has() const
	{
		for(const auto& action_ptr : this->actions)
		{
			if(action_ptr->get_id() == ID)
			{
				return true;
			}
		}
		return false;
	}

	template<ActionID ID>
	const Action<ID>* ActionEntity::get() const
	{
		for(const auto& action_ptr : this->actions)
		{
			if(action_ptr->get_id() == ID)
			{
				return static_cast<Action<ID>*>(action_ptr.get());
			}
		}
		return nullptr;
	}

	template<ActionID ID>
	Action<ID>* ActionEntity::get()
	{
		for(auto& action_ptr : this->actions)
		{
			if(action_ptr->get_id() == ID)
			{
				return static_cast<Action<ID>*>(action_ptr.get());
			}
		}
		return nullptr;
	}
}
