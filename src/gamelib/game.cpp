#include "gamelib/game.hpp"
#include "gamelib/core/quadtree.hpp"
#include "gamelib/render/font.hpp"
#include "gamelib/render/quad_renderer.hpp"
#include "gamelib/render/scene_renderer.hpp"
#include "gamelib/render/text_renderer.hpp"
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
		scene_renderer srenderer;
		text_renderer trenderer{rnlib::font::lucida_sans_regular};
		actor_system actors;
		camera cam;
		static constexpr float initial_quadtree_size = 10.0f;
		actor_quadtree tree{box{tz::vec2{-1.0f, -1.0f} * initial_quadtree_size, tz::vec2{1.0f, 1.0f} * initial_quadtree_size}};
	};

	std::unique_ptr<system> sys = nullptr;

	struct dbgui_data_t
	{
		bool show_quad_renderer = false;
		bool show_text_renderer = false;
		bool show_effect_renderer = false;
		bool show_actor_system = false;
		bool debug_draw_quadtree = TZ_DEBUG;
	} dbgui_data;

	void initialise()
	{
		TZ_PROFZONE("rnlib - initialise", 0xff0077ee);
		font_system_initialise();
		sys = std::make_unique<system>();
		sys->trenderer.add("ffs harry stop committing swear words in these placeholder texts", tz::vec2{-0.5f, 0.3f}, tz::vec2::filled(0.125f));
		tz::dbgui::game_menu().add_callback([]()
		{
			ImGui::MenuItem("Quad Renderer", nullptr, &dbgui_data.show_quad_renderer);
			ImGui::MenuItem("Text Renderer", nullptr, &dbgui_data.show_text_renderer);
			ImGui::MenuItem("Effect Renderer", nullptr, &dbgui_data.show_effect_renderer);
			ImGui::MenuItem("Actor System", nullptr, &dbgui_data.show_actor_system);
		});
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
		font_system_terminate();
	}

	void render()
	{
		TZ_PROFZONE("rnlib - render", 0xff0077ee);
		tz::assert(sys != nullptr, "rnlib never initialised. please submit a bug report.");
		sys->qrenderer.set_render_data
		({
			.view = sys->cam.view(),
			.projection = sys->cam.projection()
		});
		sys->trenderer.set_render_data
		({
			.view = sys->cam.view(),
			.projection = sys->cam.projection()
		});
		mount_result mres;
		sys->qrenderer.clean();
		{
			TZ_PROFZONE("quad renderer - mount", 0xff0077ee);
			while(mres = sys->actors.mount(sys->qrenderer.quads()), mres.error == mount_error::ooq)
			{
				// if mounting fails due to not enough quads, double the number of quads (a la std::vector)
				const std::size_t quad_count = sys->qrenderer.quads().size();
				sys->qrenderer.reserve(quad_count * 2);
				tz::report("ran out of quads. increasing %zu->%zu", quad_count, quad_count * 2);
				// default assign all new quad values.
				for(std::size_t i = quad_count; i < quad_count * 2; i++)
				{
					sys->qrenderer.quads()[i] = {};
				}
			}
		}
		tz::assert(mres.error == mount_error::no_error, "unhandled mount_error. please submit a bug report.");
		if(dbgui_data.debug_draw_quadtree)
		{
			mres.count += sys->tree.debug_mount(sys->qrenderer.quads().subspan(mres.count)).count;
		}
		if(sys->actors.size())
		{
			TZ_PROFZONE("apply text to all actors temp", 0xff0077ee);
			sys->trenderer.clear();
			for(rnlib::actor& a : sys->actors.container())
			{
				tz::vec2 abscale = a.transform.get_scale();
				abscale[0] = std::abs(abscale[0]) * 0.75f;
				abscale[1] = std::abs(abscale[1]);
				tz::vec2 pos = a.transform.get_position() - abscale;
				sys->trenderer.add(a.name, pos, tz::vec2{0.05f, 0.05f} * a.transform.get_scale().length());
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
				auto [min, max] = sys->cam.get_view_bounds();
				ImGui::Text("View Bounds: {%.2f, %.2f}, {%.2f, %.2f}", min[0], min[1], max[0], max[1]);
				ImGui::Checkbox("Draw Quadtree", &dbgui_data.debug_draw_quadtree);
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
		// handle quadtree.
		{
			TZ_PROFZONE("build quadtree", 0xff0077ee);
			sys->tree.clear();
			for(const actor& a : sys->actors.container())
			{
				sys->tree.add({.uuid = a.uuid, .bounding_box = a.transform.get_bounding_box()});
			}
			sys->actors.set_intersection_state(sys->tree.find_all_intersections());
		}
		// done quadtree.

		tz::assert(sys != nullptr, "rnlib never initialised. please submit a bug report.");
		sys->actors.update(dt,
		{
			.view_bounds = sys->cam.get_view_bounds(),
			.level_bounds = {tz::vec2::zero(), tz::vec2::zero()}
		});
		sys->srenderer.update();
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
