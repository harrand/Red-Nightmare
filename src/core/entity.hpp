#ifndef REDNIGHTMARE_ENTITY_HPP
#define REDNIGHTMARE_ENTITY_HPP
#include <vector>
#include <memory>

namespace game
{
	template<typename I, typename T>
	concept ComponentInterface = requires(const I& i)
	{
		{i.get_id()} -> std::same_as<T>;
	};

	template<typename T, ComponentInterface<T> icomponent, template<T> typename Component, template<T> typename ComponentParams>
	class Entity
	{
	public:
		Entity() = default;
		template<typename... C>
		Entity(C... cs);
		Entity(const Entity<T, icomponent, Component, ComponentParams>& copy);
		Entity(Entity<T, icomponent, Component, ComponentParams>&& move) = default;
		~Entity() = default;
		Entity& operator=(const Entity<T, icomponent, Component, ComponentParams>& rhs);
		Entity& operator=(Entity<T, icomponent, Component, ComponentParams>&& rhs) = default;

		template<T t>
		bool add(ComponentParams<t> params = {});
		template<T t>
		bool set(ComponentParams<t> params = {});
		template<T t>
		bool has() const;
		template<T t>
		bool remove();
		template<T t>
		const Component<t>* get() const;
		template<T t>
		Component<t>* get();

		std::size_t size() const;
		bool empty() const;
		void clear();

		void transfer_components(Entity<T, icomponent, Component, ComponentParams>& other);
		void copy_components(Entity<T, icomponent, Component, ComponentParams>& other);

		virtual void update(){}
	protected:
		std::vector<std::unique_ptr<icomponent>> components;
	};
}
#include "entity.inl"

#endif // REDNIGHTMARE_ENTITY_HPP
