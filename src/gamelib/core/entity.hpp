#ifndef RNLIB_CORE_ENTITY_HPP
#define RNLIB_CORE_ENTITY_HPP
#include <type_traits>
#include <vector>
#include <memory>

namespace rnlib
{
	template<typename I, typename T>
	concept component_interface = requires(const I& i)
	{
		{i.get_component_id()} -> std::same_as<T>;
	};

	template<typename T, component_interface<T> icomponent, template<T> typename component, template<T> typename component_params>
	class entity
	{
	public:
		entity() = default;
		template<typename... C>
		entity(C... cs);
		entity(const entity<T, icomponent, component, component_params>& copy);
		entity(entity<T, icomponent, component, component_params>&& move) = default;
		~entity() = default;
		entity& operator=(const entity<T, icomponent, component, component_params>& rhs);
		entity& operator=(entity<T, icomponent, component, component_params>&& rhs) = default;

		template<T t>
		bool add_component(component_params<t> params = {});
		template<T t>
		bool set_component(component_params<t> params = {});
		template<T t>
		bool has_component() const;
		template<T t>
		bool remove_component();
		template<T t>
		const component<t>* get_component() const;
		template<T t>
		component<t>* get_component();

		std::size_t size() const;
		bool empty() const;
		void clear();

		void transfer_components(entity<T, icomponent, component, component_params>& other);
		void copy_components(entity<T, icomponent, component, component_params>& other);

		virtual void update(){}
	protected:
		std::vector<std::unique_ptr<icomponent>> components;
	};
}
#include "gamelib/core/entity.inl"

#endif // RNLIB_CORE_ENTITY_HPP
