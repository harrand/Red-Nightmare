#ifndef RNLIB_GAMEPLAY_ACTOR_HPP
#define RNLIB_GAMEPLAY_ACTOR_HPP
#include "gamelib/core/entity.hpp"
#include "gamelib/core/transform.hpp"
#include "tz/core/memory/clone.hpp"

namespace rnlib
{
	enum class actor_component_id
	{

	};

	template<actor_component_id ID>
	struct actor_component_params{};

	class iactor_component : public tz::unique_cloneable<iactor_component>
	{
	public:
		virtual constexpr actor_component_id get_id() const = 0;
		virtual ~iactor_component() = default;
	private:
	};

	template<actor_component_id ID>
	class actor_component : public iactor_component
	{
	public:
		actor_component(actor_component_params<ID> params = {}): params(std::move(params)){}
		[[nodiscard]] virtual std::unique_ptr<iactor_component> unique_clone() const
		{
			return static_cast<std::unique_ptr<iactor_component>>(std::make_unique<actor_component<ID>>(*this));
		}
		virtual constexpr actor_component_id get_id() const override{return ID;}
		const actor_component_params<ID>& data() const{return this->params;}
		actor_component_params<ID>& data() {return this->params;}
	private:
		actor_component_params<ID> params;
	};

	class actor_entity : public rnlib::entity<actor_component_id, iactor_component, actor_component, actor_component_params>
	{
		using rnlib::entity<actor_component_id, iactor_component, actor_component, actor_component_params>::entity;
	};

	struct actor
	{
		actor_entity entity = {};
		transform_t transform = {};
	};
}

#endif // RNLIB_GAMEPLAY_ACTOR_HPP
