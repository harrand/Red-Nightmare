#include "gamelib/game.hpp"
#include "gamelib/render/font.hpp"
#include "gamelib/render/quad_renderer.hpp"
#include "gamelib/render/scene_renderer.hpp"
#include "gamelib/render/text_renderer.hpp"
#include "gamelib/gameplay/scene/scene.hpp"
#include "tz/core/profile.hpp"
#include "tz/dbgui/dbgui.hpp"
#include <memory>

namespace rnlib
{
	struct system
	{
		quad_renderer qrenderer;
		scene_renderer srenderer;
		text_renderer trenderer{rnlib::font::lucida_sans_regular};
		scene current_scene = {};
	};

	std::unique_ptr<system> sys = nullptr;

	struct dbgui_data_t
	{
		bool show_quad_renderer = false;
		bool show_text_renderer = false;
		bool show_effect_renderer = false;
	} dbgui_data;

	void initialise()
	{
		TZ_PROFZONE("rnlib - initialise", 0xff0077ee);
		font_system_initialise();
		tz::dbgui::game_menu().add_callback([]()
		{
			ImGui::MenuItem("Quad Renderer", nullptr, &dbgui_data.show_quad_renderer);
			ImGui::MenuItem("Text Renderer", nullptr, &dbgui_data.show_text_renderer);
			ImGui::MenuItem("Effect Renderer", nullptr, &dbgui_data.show_effect_renderer);
		});
		sys = std::make_unique<system>();
		tz::gl::get_device().render_graph().add_dependencies(sys->trenderer.get(), sys->qrenderer.get());
		tz::gl::get_device().render_graph().add_dependencies(sys->qrenderer.get(), sys->srenderer.get_layer_renderer());
		tz::gl::get_device().render_graph().timeline = {sys->srenderer.get_layer_renderer(), sys->qrenderer.get(), sys->trenderer.get()};
		auto effects = sys->srenderer.get_effects();
		// quad renderer depends on all effects being finished (?)
		for(tz::gl::renderer_handle effect : effects)
		{
			tz::gl::get_device().render_graph().add_dependencies(sys->srenderer.get_layer_renderer(), effect);
			// each effect goes to the front of the timeline as they happen first.
			auto& tl = tz::gl::get_device().render_graph().timeline;
			tl.insert(tl.begin(), static_cast<unsigned int>(static_cast<tz::hanval>(effect)));
		}
	}

	void terminate()
	{
		TZ_PROFZONE("rnlib - terminate", 0xff0077ee);
		sys = nullptr;
		font_system_terminate();
	}

	void render()
	{
		TZ_PROFZONE("rnlib - render", 0xff0077ee);
		tz::assert(sys != nullptr, "rnlib never initialised. please submit a bug report.");
		const camera& cam = sys->current_scene.get_camera();
		sys->qrenderer.set_render_data
		({
			.view = cam.view(),
			.projection = cam.projection()
		});
		sys->trenderer.set_render_data
		({
			.view = cam.view(),
			.projection = cam.projection()
		});
		mount_result mres;
		sys->qrenderer.clean();
		while(mres = sys->current_scene.mount(sys->qrenderer.quads()), mres.error == mount_error::ooq)
		{
			auto quad_count = sys->qrenderer.quads().size();
			tz::report("ran out of quads. increasing %zu->%zu", quad_count, quad_count * 2);
			sys->qrenderer.reserve(quad_count * 2);
			sys->qrenderer.clean();
		}
		tz::assert(mres.error == mount_error::no_error, "unhandled mount_error. please submit a bug report.");
		if(sys->current_scene.size())
		{
			TZ_PROFZONE("Actor Label Text", 0xff0077ee);
			sys->trenderer.clear();
			for(const rnlib::actor& a : sys->current_scene.get_actor_system().container())
			{
				if(a.entity.has_component<actor_component_id::label>())
				{
					auto& label = a.entity.get_component<actor_component_id::label>()->data();
					const char* name = a.name;
					if(!label.text.empty())
					{
						name = label.text.c_str();
					}
					tz::vec2 abscale = a.transform.get_scale();
					abscale[0] = std::abs(abscale[0]) * 0.75f;
					abscale[1] = std::abs(abscale[1]);
					tz::vec2 pos = a.transform.get_position() - abscale;
					sys->trenderer.add(name, pos, tz::vec2{0.05f, 0.05f} * a.transform.get_scale().length(), label.colour);
				}
			}
		}
		sys->qrenderer.render(mres.count);
		sys->trenderer.render();
		tz::gl::get_device().render();
	}

	void dbgui()
	{
		TZ_PROFZONE("rnlib - dbgui", 0xff0077ee);
		tz::assert(sys != nullptr, "rnlib never initialised. please submit a bug report.");
		if(dbgui_data.show_quad_renderer)
		{
			ImGui::Begin("Quad Renderer", &dbgui_data.show_quad_renderer);
			{
				auto [min, max] = sys->current_scene.get_camera().get_view_bounds();
				ImGui::Text("View Bounds: {%.2f, %.2f}, {%.2f, %.2f}", min[0], min[1], max[0], max[1]);
			}
			sys->qrenderer.dbgui();
			ImGui::End();
		}
		if(dbgui_data.show_text_renderer)
		{
			ImGui::Begin("Text Renderer", &dbgui_data.show_text_renderer);
			sys->trenderer.dbgui();
			ImGui::End();
		}
		if(dbgui_data.show_effect_renderer)
		{
			ImGui::Begin("Effect Renderer", &dbgui_data.show_effect_renderer);
			sys->srenderer.dbgui();
			ImGui::End();
		}
		sys->current_scene.dbgui();
	}

	void update(float dt)
	{
		TZ_PROFZONE("rnlib - update", 0xff0077ee);

		tz::assert(sys != nullptr, "rnlib never initialised. please submit a bug report.");
		sys->current_scene.update(dt);
		sys->srenderer.update();
	}
}
