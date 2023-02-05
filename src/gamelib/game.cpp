#include "gamelib/game.hpp"
#include "gamelib/render/quad_renderer.hpp"
#include "gamelib/render/camera.hpp"
#include "gamelib/gameplay/actor/system.hpp"
#include "tz/core/profile.hpp"
#include "tz/dbgui/dbgui.hpp"
#include <memory>

namespace rnlib
{
	struct system
	{
		quad_renderer qrenderer;
		actor_system actors;
		camera cam;
	};

	std::unique_ptr<system> sys = nullptr;

	struct dbgui_data_t
	{
		bool show_quad_renderer = false;
		bool show_actor_system = false;
	} dbgui_data;

	void initialise()
	{
		TZ_PROFZONE("rnlib - initialise", 0xff0077ee);
		sys = std::make_unique<system>();
		tz::assert(sys->qrenderer.quads().size() == 512);
		sys->qrenderer.reserve(1024);
		tz::assert(sys->qrenderer.quads().size() == 1024);
		tz::dbgui::game_menu().add_callback([]()
		{
			ImGui::MenuItem("Quad Renderer", nullptr, &dbgui_data.show_quad_renderer);
			ImGui::MenuItem("Actor System", nullptr, &dbgui_data.show_actor_system);
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
		sys->qrenderer.clean();
		sys->qrenderer.set_render_data
		({
			.view = sys->cam.view(),
			.projection = sys->cam.projection()
		});
		sys->actors.mount(sys->qrenderer.quads());
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
		if(dbgui_data.show_actor_system)
		{
			ImGui::Begin("Actor System", &dbgui_data.show_actor_system);
			sys->actors.dbgui();
			ImGui::End();
		}
	}

	void handle_camera_zoom();

	void update(float dt)
	{
		TZ_PROFZONE("rnlib - update", 0xff0077ee);
		tz::assert(sys != nullptr, "rnlib never initialised. please submit a bug report.");
		sys->actors.update(dt);
		handle_camera_zoom();
	}

	void handle_camera_zoom()
	{
		static int scroll_cache = tz::window().get_mouse_state().wheel_position;
		int scroll = tz::window().get_mouse_state().wheel_position;
		if(tz::dbgui::claims_mouse())
		{
			scroll_cache = scroll;
			return;
		}
		if(scroll > scroll_cache)
		{
			sys->cam.zoom -= 0.1f;
		}
		else if(scroll < scroll_cache)
		{
			sys->cam.zoom += 0.1f;
		}
		scroll_cache = scroll;
	}
}
