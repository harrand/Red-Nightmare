namespace game
{
	#define ENTITY_IMPL Entity<T, IComponent, Component, ComponentParams>
	#define TEMPLATE_MAGIC template<typename T, ComponentInterface<T> IComponent, template<T> typename Component, template<T> typename ComponentParams>

	TEMPLATE_MAGIC
	template<typename... C>
	ENTITY_IMPL::Entity(C... cs):
	components()
	{
		auto tup = std::forward_as_tuple(cs...);

		int i = 0;
		([&]
		{
			auto comp_clone = std::make_unique<decltype(cs)>(cs.data());
			auto* comp_released = comp_clone.release();
			this->components.push_back(std::unique_ptr<IComponent>{static_cast<IComponent*>(comp_released)});
		} (), ...);
	}

	TEMPLATE_MAGIC
	template<T t>
	bool ENTITY_IMPL::add(ComponentParams<t> params)
	{
		if(this->has<t>())
		{
			return false;
		}
		auto derived_ptr = std::make_unique<Component<t>>(params);
		Component<t>* derived_owner = derived_ptr.release();
		this->components.push_back(std::unique_ptr<IComponent>(static_cast<IComponent*>(derived_owner)));
		return true;
	}

	TEMPLATE_MAGIC
	template<T t>
	bool ENTITY_IMPL::set(ComponentParams<t> params)
	{
		if(this->has<t>())
		{
			*this->get<t>() = Component<t>{params};
			return false;
		}
		else
		{
			this->add<t>(params);
			return true;
		}
	}

	TEMPLATE_MAGIC
	template<T t>
	bool ENTITY_IMPL::has() const
	{
		for(const auto& comp_ptr : this->components)
		{
			if(comp_ptr->get_id() == t)
			{
				return true;
			}
		}
		return false;
	}

	TEMPLATE_MAGIC
	template<T t>
	const Component<t>* ENTITY_IMPL::get() const
	{
		for(const auto& comp_ptr : this->components)
		{
			if(comp_ptr->get_id() == t)
			{
				return static_cast<const Component<t>*>(comp_ptr.get());
			}
		}
		return nullptr;
	}

	TEMPLATE_MAGIC
	template<T t>
	Component<t>* ENTITY_IMPL::get()
	{
		for(auto& comp_ptr : this->components)
		{
			if(comp_ptr->get_id() == t)
			{
				return static_cast<Component<t>*>(comp_ptr.get());
			}
		}
		return nullptr;
	}

	TEMPLATE_MAGIC
	std::size_t ENTITY_IMPL::size() const
	{
		return this->components.size();
	}

	TEMPLATE_MAGIC
	bool ENTITY_IMPL::empty() const
	{
		return this->size() == 0;
	}
}
