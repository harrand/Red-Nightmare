#ifndef REDNIGHTMARE_ENTITY_HPP
#define REDNIGHTMARE_ENTITY_HPP
#include <vector>
#include <memory>

namespace game
{
	template<typename I, typename T>
	concept ComponentInterface = requires(I i)
	{
		{i.get_id()} -> std::same_as<T>;
	};
	template<typename T, ComponentInterface<T> IComponent, template<T> typename Component, template<T> typename ComponentParams>
	class Entity
	{
	public:
		Entity() = default;
		Entity(const Entity<T, IComponent, Component, ComponentParams>& copy) = delete;
		Entity(Entity<T, IComponent, Component, ComponentParams>&& move) = default;
		~Entity() = default;
		Entity& operator=(const Entity<T, IComponent, Component, ComponentParams>& rhs) = delete;
		Entity& operator=(Entity<T, IComponent, Component, ComponentParams>&& rhs) = default;

		template<T t>
		bool add(ComponentParams<t> params = {});
		template<T t>
		bool set(ComponentParams<t> params = {});
		template<T t>
		bool has() const;
		template<T t>
		const Component<t>* get() const;
		template<T t>
		Component<t>* get();

		std::size_t size() const;
		bool empty() const;

		virtual void update(){}
	protected:
		std::vector<std::unique_ptr<IComponent>> components;
	};
}
#include "entity.inl"

#endif // REDNIGHTMARE_ENTITY_HPP
