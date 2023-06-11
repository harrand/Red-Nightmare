#ifndef RNLIB_GAMEPLAY_SCENE_HPP
#define RNLIB_GAMEPLAY_SCENE_HPP
#include "gamelib/gameplay/actor/system.hpp"
#include "gamelib/render/camera.hpp"
#include "gamelib/render/quad_renderer.hpp"
#include "gamelib/core/mount.hpp"
#include "gamelib/core/quadtree.hpp"

namespace rnlib
{
	class scene
	{
	public:
		scene();
		void update(float dt);
		void dbgui();
		const camera& get_camera() const;
		const actor_system& get_actor_system() const;
		std::size_t size() const;
		mount_result mount(std::span<quad_renderer::quad_data> quads);
	private:
		void rebuild_quadtree();
		void handle_camera_zoom();
		void handle_camera_position();

		actor_system actors = {};
		camera cam = {};
		std::pair<tz::vec2, tz::vec2> level_bounds = {{-10.0f, -10.0f}, {10.0f, 10.0f}};
		static constexpr float initial_quadtree_size = 10.0f;
		actor_quadtree tree{box{tz::vec2{-1.0f, -1.0f} * initial_quadtree_size, tz::vec2{1.0f, 1.0f} * initial_quadtree_size}};
		bool actor_dbgui = false;
		bool tree_dbgui = false;
		bool scene_dbgui = false;
	};
}

#endif // RNLIB_GAMEPLAY_SCENE_HPP
