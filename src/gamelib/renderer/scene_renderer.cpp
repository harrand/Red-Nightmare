#include "gamelib/renderer/scene_renderer.hpp"

namespace game::render
{
	scene_renderer::scene_renderer()
	{
		this->animation.append_to_render_graph();
	}
}