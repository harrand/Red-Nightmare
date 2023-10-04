#ifndef RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP
#define RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP
#include "tz/ren/animation.hpp"

namespace game::render
{
	class scene_renderer
	{
	public:
		scene_renderer();
		using entry = tz::ren::animation_renderer::asset_package;

		enum class model
		{
			quad,
			humanoid
		};

	private:
		tz::ren::animation_renderer animation;
	};
}

#endif // RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP