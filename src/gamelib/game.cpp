#include "gamelib/game.hpp"
#include "gamelib/render/quad_renderer.hpp"
#include "tz/core/profile.hpp"
#include <memory>

namespace rnlib
{
	struct system
	{
		quad_renderer qrenderer;
	};

	std::unique_ptr<system> sys = nullptr;

	void initialise()
	{
		TZ_PROFZONE("rnlib - initialise", 0xff0077ee);
		sys = std::make_unique<system>();
	}

	void terminate()
	{
		TZ_PROFZONE("rnlib - terminate", 0xff0077ee);

	}

	void render()
	{
		TZ_PROFZONE("rnlib - render", 0xff0077ee);
		sys->qrenderer.render();
	}

	void dbgui()
	{
		TZ_PROFZONE("rnlib - dbgui", 0xff0077ee);

	}
	void update(float dt)
	{
		TZ_PROFZONE("rnlib - update", 0xff0077ee);

	}
}
