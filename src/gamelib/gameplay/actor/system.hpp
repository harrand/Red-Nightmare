#ifndef RNLIB_GAMEPLAY_ACTOR_SYSTEM_HPP
#define RNLIB_GAMEPLAY_ACTOR_SYSTEM_HPP
#include "gamelib/gameplay/actor/entity.hpp"
#include "gamelib/render/quad_renderer.hpp"

namespace rnlib
{
	class actor_system
	{
	public:
		actor_system() = default;
		std::size_t mount(std::span<quad_renderer::quad_data> quads);
		void dbgui();
	private:
		std::vector<actor> entities = {};
	};
}

#endif // RNLIB_GAMEPLAY_ACTOR_SYSTEM_HPP
