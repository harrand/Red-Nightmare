#ifndef RNLIB_GAMEPLAY_ACTOR_HPP
#define RNLIB_GAMEPLAY_ACTOR_HPP
#include "gamelib/core/entity.hpp"
#include "gamelib/core/transform.hpp"
#include "gamelib/render/quad_renderer.hpp"
#include "tz/core/memory/clone.hpp"
#include <limits>

namespace rnlib
{
	// all traits (ECS components) that an actor could possibly have.
	enum class actor_component_id
	{
		sprite,
		_count
	};

	// each trait above must have a corresponding name string in the following array (or enjoy a cryptic crash)
	constexpr std::array<const char*, static_cast<int>(actor_component_id::_count)> actor_component_id_name{"Sprite"};

	// ECS boilerplate begin.
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
	public:
		using rnlib::entity<actor_component_id, iactor_component, actor_component, actor_component_params>::entity;
		void dbgui();
	};

	// ecs - system impls:
	template<actor_component_id ID>
	inline void actor_component_mount(const actor_component<ID>& component, quad_renderer::quad_data& quad){}
	#include "gamelib/gameplay/actor/components/sprite.hpp"
	// ecs boilerplate end.

	enum class actor_type
	{
		undefined,
		player_akhara
	};

	struct actor
	{
		static std::size_t uuid_count;
		static constexpr std::size_t nullid = std::numeric_limits<std::size_t>::max();
		void dbgui();
		void mount(quad_renderer::quad_data& quad) const;

		actor_entity entity = {};
		transform_t transform = {};
		const char* name = "Untitled";
		std::size_t uuid = uuid_count++;
		actor_type type = actor_type::undefined;
	};

	actor create_actor(actor_type type);
}

#endif // RNLIB_GAMEPLAY_ACTOR_HPP
