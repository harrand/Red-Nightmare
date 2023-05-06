#ifndef RNLIB_GAMEPLAY_ACTOR_SYSTEM_HPP
#define RNLIB_GAMEPLAY_ACTOR_SYSTEM_HPP
#include "gamelib/gameplay/actor/entity.hpp"
#include "gamelib/gameplay/actor/combat_log.hpp"
#include "gamelib/core/quadtree.hpp"
#include "gamelib/render/quad_renderer.hpp"
#include "gamelib/core/mount.hpp"

namespace rnlib
{
	class actor_system
	{
	public:
		actor_system() = default;

		actor& add(actor_type t);

		const actor* find(std::size_t uuid) const;
		actor* find(std::size_t uuid);
		void remove(std::size_t uuid);
		mount_result mount(std::span<quad_renderer::quad_data> quads);
		std::size_t size() const;
		std::span<actor> container();
		std::span<const actor> container() const;
		const combat_log& get_combat_log() const;
		void add_to_combat_log(combat_event evt);
		void set_intersection_state(actor_quadtree::intersection_state_t state);
		void update(float dt, update_context ctx);
		void dbgui();
	private:
		void update_one(std::size_t eid, float dt);
		void update_n(std::size_t eid_begin, std::size_t n, float dt);
		void collision_response();
		void resolve_collision(std::size_t uuid_a, std::size_t uuid_b);
		void send_combat_text();
		std::vector<actor> entities = {};
		std::vector<std::size_t> entities_to_delete = {};
		std::vector<actor> entities_to_add = {};
		combat_log log = {};
		std::vector<combat_event> combat_events_this_frame = {};
		actor_quadtree::intersection_state_t intersection_state = {};
	};
}

#endif // RNLIB_GAMEPLAY_ACTOR_SYSTEM_HPP
