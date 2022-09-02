namespace game
{
	template<ActionID ID>
	bool ActionEntity::add(ActionParams<ID> params)
	{
		if(this->has<ID>())
		{
			return false;
		}
		auto derived_ptr = std::make_unique<Action<ID>>(params);
		Action<ID>* derived_owner = derived_ptr.release();
		this->actions.push_back(std::unique_ptr<IAction>(static_cast<IAction*>(derived_owner)));
		return true;
	}

	template<ActionID ID>
	bool ActionEntity::set(ActionParams<ID> params)
	{
		if(this->has<ID>())
		{
			this->get<ID>()->data() = params;
			this->get<ID>()->set_is_complete(false);
			return false;
		}
		else
		{
			this->add<ID>(params);
			return true;
		}
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
