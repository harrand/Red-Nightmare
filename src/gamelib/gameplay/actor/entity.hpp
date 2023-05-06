#ifndef RNLIB_GAMEPLAY_ACTOR_HPP
#define RNLIB_GAMEPLAY_ACTOR_HPP
#include "gamelib/core/entity.hpp"
#include "gamelib/core/mount.hpp"
#include "gamelib/core/transform.hpp"
#include "gamelib/render/animation.hpp"
#include "gamelib/render/quad_renderer.hpp"
#include "gamelib/render/image.hpp"
#include "gamelib/gameplay/actor/actions/action.hpp"
#include "gamelib/gameplay/actor/spell/spell.hpp" // for actor_component_id::cast
#include "gamelib/gameplay/actor/combat_log.hpp"
#include "tz/core/memory/clone.hpp"
#include "tz/core/profile.hpp" // for profiler instrumentation for components.
#include "tz/core/time.hpp" // for actor_component_id::cast
#include "tz/wsi/mouse.hpp" // actor_component_id::humanoid_skeleton cast animation (dev debug)
#include "tz/wsi/keyboard.hpp"
#include "tz/dbgui/dbgui.hpp" // actor_component_id::humanoid_skeleton cast animation (dev debug)
#include <optional>
#include <limits>
#include <bit> // actor_component_id::motion
#include <unordered_map> // actor_component_id::keyboard_control

namespace rnlib
{
	// all traits (ECS components) that an actor could possibly have.
	enum class actor_component_id
	{
		// actor will perform an action upon a certain event.
		action_listener,
		// actor has a sprite, meaning it can be drawn in the world as a textured quad.
		sprite,
		// actor has an animation, which means its sprite texture changes over time. requires `sprite`
		animation,
		// actor collides with others.
		collide,
		// actor is casting some kind of spell/ability.
		cast,
		// actor has the concept of health, damage and death.
		damageable,
		// actor has the concept of proper movement.
		motion,
		// actor is controlled by a keyboard. requires `motion`
		keyboard_control,
		// actor contains a text label to be displayed above it.
		label,
		// actor has a set of animations fitting a humanoid skeleton. requires `animation`, `motion`.
		humanoid_skeleton,
		skin,
		_count
	};

	// each trait above must have a corresponding name string in the following array (or enjoy a cryptic crash)
	constexpr std::array<const char*, static_cast<int>(actor_component_id::_count)> actor_component_id_name{"Action Listener", "Sprite", "Animation", "Collide", "Cast", "Damageable", "Motion", "Keyboard Control", "Label", "Humanoid Skeleton", "Skin"};

	// ECS boilerplate begin.
	template<actor_component_id ID>
	struct actor_component_params{};

	struct actor;

	class iactor_component : public tz::unique_cloneable<iactor_component>
	{
	public:
		virtual constexpr actor_component_id get_id() const = 0;
		virtual ~iactor_component() = default;
		virtual mount_result mount(std::span<quad_renderer::quad_data> quads) = 0;
		virtual void update(float dt, actor& actor) = 0;
		virtual void dbgui() = 0;
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
		virtual mount_result mount(std::span<quad_renderer::quad_data> quads) override;
		virtual void update(float dt, actor& actor) override;
		virtual void dbgui() override;
		const actor_component_params<ID>& data() const{return this->params;}
		actor_component_params<ID>& data() {return this->params;}
	private:
		actor_component_params<ID> params;
	};

	class actor_entity : public rnlib::entity<actor_component_id, iactor_component, actor_component, actor_component_params>
	{
	public:
		using rnlib::entity<actor_component_id, iactor_component, actor_component, actor_component_params>::entity;
		mount_result mount(std::span<quad_renderer::quad_data> quads) const;
		void update(float dt, actor& actor);
		void dbgui();
	};

	// ecs - system impls:
	template<actor_component_id ID>
	inline mount_result actor_component_mount(const actor_component<ID>& component, std::span<quad_renderer::quad_data> quads){return {};}

	template<actor_component_id ID>
	inline void actor_component_update(actor_component<ID>& component, float dt, actor& actor){}

	template<actor_component_id ID>
	inline void actor_component_dbgui(actor_component<ID>& component){}

	
	template<actor_component_id ID>
	mount_result actor_component<ID>::mount(std::span<quad_renderer::quad_data> quads)
	{
		return actor_component_mount<ID>(*this, quads);
	}

	template<actor_component_id ID>
	void actor_component<ID>::update(float dt, actor& actor)
	{
		actor_component_update<ID>(*this, dt, actor);
	}

	template<actor_component_id ID>
	void actor_component<ID>::dbgui()
	{
		ImGui::Text("== %s ==", actor_component_id_name[static_cast<int>(ID)]);
		actor_component_dbgui<ID>(*this);
	}
	// ecs boilerplate end.

	enum class actor_type
	{
		undefined,
		dev_barrier,
		doctor_michael_morbius,
		player_melistra,
		player_benedict,
		randomius,
		the_black_knight,
		unknown,
		_count
	};

	struct actor
	{
		static std::size_t uuid_count;
		static constexpr std::size_t nullid = std::numeric_limits<std::size_t>::max();
		void dbgui();
		void update(float dt);
		mount_result mount(std::span<quad_renderer::quad_data> quads) const;

		actor_entity entity = {};
		action_entity actions = {};
		transform_t transform = {};
		const char* name = "Untitled";
		std::size_t uuid = uuid_count++;
		actor_type type = actor_type::undefined;
		unsigned int layer = 0;
		std::size_t target = nullid;
	};

	actor create_actor(actor_type type);



	// component implementations begin.
	#include "gamelib/gameplay/actor/components/action_listener.hpp"
	#include "gamelib/gameplay/actor/components/sprite.hpp"
	#include "gamelib/gameplay/actor/components/animation.hpp"
	#include "gamelib/gameplay/actor/components/collide.hpp"
	#include "gamelib/gameplay/actor/components/damageable.hpp"
	#include "gamelib/gameplay/actor/components/motion.hpp"
	#include "gamelib/gameplay/actor/components/keyboard_control.hpp"
	#include "gamelib/gameplay/actor/components/label.hpp"
	#include "gamelib/gameplay/actor/components/cast.hpp"
	#include "gamelib/gameplay/actor/components/humanoid_skeleton.hpp"

	// prefab implementations.
	namespace prefab
	{
		#include "gamelib/gameplay/actor/prefabs/human.chest.prefab"
		#include "gamelib/gameplay/actor/prefabs/human.eyes.prefab"
		#include "gamelib/gameplay/actor/prefabs/human.feet.prefab"
		#include "gamelib/gameplay/actor/prefabs/human.hair.prefab"
		#include "gamelib/gameplay/actor/prefabs/human.helm.prefab"
	}

	#include "gamelib/gameplay/actor/components/skin.hpp"
	// component implementations end.

}

#endif // RNLIB_GAMEPLAY_ACTOR_HPP
