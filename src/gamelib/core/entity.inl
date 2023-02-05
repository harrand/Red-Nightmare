#include <algorithm>

namespace rnlib
{
	#define ENTITY_IMPL entity<T, icomponent, component, component_params>
	#define TEMPLATE_MAGIC template<typename T, component_interface<T> icomponent, template<T> typename component, template<T> typename component_params>

	TEMPLATE_MAGIC
	template<typename... C>
	ENTITY_IMPL::entity(C... cs):
	components()
	{
		int i = 0;
		([&]
		{
			auto comp_clone = std::make_unique<decltype(cs)>(std::move(cs.data()));
			auto* comp_released = comp_clone.release();
			this->components.push_back(std::unique_ptr<icomponent>{static_cast<icomponent*>(comp_released)});
		} (), ...);
	}

	TEMPLATE_MAGIC
	ENTITY_IMPL::entity(const ENTITY_IMPL& copy)
	{
		for(const auto& comp_ptr : copy.components)
		{
			this->components.push_back(comp_ptr->unique_clone());
		}
	}

	TEMPLATE_MAGIC
	ENTITY_IMPL& ENTITY_IMPL::operator=(const ENTITY_IMPL& rhs)
	{
		this->clear();
		for(const auto& comp_ptr : rhs.components)
		{
			this->components.push_back(comp_ptr->unique_clone());
		}
	}

	TEMPLATE_MAGIC
	template<T t>
	bool ENTITY_IMPL::add_component(component_params<t> params)
	{
		if(this->has_component<t>())
		{
			return false;
		}
		auto derived_ptr = std::make_unique<component<t>>(params);
		component<t>* derived_owner = derived_ptr.release();
		this->components.push_back(std::unique_ptr<icomponent>(static_cast<icomponent*>(derived_owner)));
		return true;
	}

	TEMPLATE_MAGIC
	template<T t>
	bool ENTITY_IMPL::set_component(component_params<t> params)
	{
		if(this->has_component<t>())
		{
			*this->get_component<t>() = component<t>{params};
			return false;
		}
		else
		{
			this->add_component<t>(params);
			return true;
		}
	}

	TEMPLATE_MAGIC
	template<T t>
	bool ENTITY_IMPL::has_component() const
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
	bool ENTITY_IMPL::remove_component()
	{
		auto iter = this->components.erase(std::remove_if(this->components.begin(), this->components.end(),
		[](const auto& component_ptr)
		{
			return component_ptr->get_id() == t;
		}));
		return iter != this->components.end();
	}

	TEMPLATE_MAGIC
	template<T t>
	const component<t>* ENTITY_IMPL::get_component() const
	{
		for(const auto& comp_ptr : this->components)
		{
			if(comp_ptr->get_id() == t)
			{
				return static_cast<const component<t>*>(comp_ptr.get());
			}
		}
		return nullptr;
	}

	TEMPLATE_MAGIC
	template<T t>
	component<t>* ENTITY_IMPL::get_component()
	{
		for(auto& comp_ptr : this->components)
		{
			if(comp_ptr->get_id() == t)
			{
				return static_cast<component<t>*>(comp_ptr.get());
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

	TEMPLATE_MAGIC
	void ENTITY_IMPL::clear()
	{
		this->components.clear();
	}

	TEMPLATE_MAGIC
	void ENTITY_IMPL::transfer_components(entity<T, icomponent, component, component_params>& other)
	{
		for(auto& comp_ptr : this->components)
		{
			
			auto iter = std::find_if(other.components.begin(), other.components.end(), [&comp_ptr](const auto& rhs){return rhs->get_id() == comp_ptr->get_id();});
			if(iter != other.components.end())
			{
				*iter = std::move(comp_ptr);
			}
			else
			{
				other.components.push_back(std::move(comp_ptr));
			}
		}
		this->clear();
	}

	TEMPLATE_MAGIC
	void ENTITY_IMPL::copy_components(entity<T, icomponent, component, component_params>& other)
	{
		for(const auto& comp_ptr : this->components)
		{
			auto iter = std::find_if(other.components.begin(), other.components.end(), [&comp_ptr](const auto& rhs){return rhs->get_id() == comp_ptr->get_id();});
			if(iter != other.components.end())
			{
				*iter = comp_ptr->unique_clone();
			}
			else
			{
				other.components.push_back(comp_ptr->unique_clone());
			}
		}
	}
}
