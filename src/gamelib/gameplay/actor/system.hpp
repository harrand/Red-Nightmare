#ifndef RNLIB_GAMEPLAY_ACTOR_SYSTEM_HPP
#define RNLIB_GAMEPLAY_ACTOR_SYSTEM_HPP
#include "gamelib/gameplay/actor/entity.hpp"
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
		mount_result mount(std::span<quad_renderer::quad_data> quads);
		std::size_t size() const;
		std::span<actor> container();
		std::span<const actor> container() const;
		void update(float dt);
		void dbgui();
	private:
		void update_one(std::size_t eid, float dt);
		void update_n(std::size_t eid_begin, std::size_t n, float dt);
		std::vector<actor> entities = {};
	};
}

#endif // RNLIB_GAMEPLAY_ACTOR_SYSTEM_HPP
