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
		const actor* find(std::size_t uuid) const;
		actor* find(std::size_t uuid);
		mount_result mount(std::span<quad_renderer::quad_data> quads);
		void update(float dt);
		void dbgui();
	private:
		std::vector<actor> entities = {};
	};
}

#endif // RNLIB_GAMEPLAY_ACTOR_SYSTEM_HPP
