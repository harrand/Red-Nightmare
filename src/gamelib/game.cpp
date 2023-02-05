#include "gamelib/game.hpp"
#include "gamelib/gameplay/actor.hpp"
#include "gamelib/render/quad_renderer.hpp"
#include "tz/core/profile.hpp"
#include "tz/dbgui/dbgui.hpp"
#include <memory>

namespace rnlib
{
	struct system
	{
		quad_renderer qrenderer;
	};

	std::unique_ptr<system> sys = nullptr;

	struct dbgui_data_t
	{
		bool show_quad_renderer = false;
	} dbgui_data;

	void initialise()
	{
		TZ_PROFZONE("rnlib - initialise", 0xff0077ee);
		sys = std::make_unique<system>();
		tz::dbgui::game_menu().add_callback([]()
		{
			ImGui::MenuItem("Quad Renderer", nullptr, &dbgui_data.show_quad_renderer);
		});
	}

	void terminate()
	{
		TZ_PROFZONE("rnlib - terminate", 0xff0077ee);

	}

	void render()
	{
		TZ_PROFZONE("rnlib - render", 0xff0077ee);
		tz::assert(sys != nullptr, "rnlib never initialised. please submit a bug report.");

		sys->qrenderer.render();
	}

	void dbgui()
	{
		TZ_PROFZONE("rnlib - dbgui", 0xff0077ee);
		tz::assert(sys != nullptr, "rnlib never initialised. please submit a bug report.");
		if(dbgui_data.show_quad_renderer)
		{
			ImGui::Begin("Quad Renderer", &dbgui_data.show_quad_renderer);
			sys->qrenderer.dbgui();
			ImGui::End();
		}
	}
	void update(float dt)
	{
		TZ_PROFZONE("rnlib - update", 0xff0077ee);
		tz::assert(sys != nullptr, "rnlib never initialised. please submit a bug report.");

	}
}
