#include "gamelib/gameplay/scene/scene.hpp"
#include "tz/dbgui/dbgui.hpp"

namespace rnlib
{
	scene::scene()
	{
		tz::dbgui::game_menu().add_callback([this]()
		{
			ImGui::MenuItem("Actor System", nullptr, &this->actor_dbgui);
			ImGui::MenuItem("Quadtree", nullptr, &this->tree_dbgui);
		});
	}

	void scene::update(float dt)
	{
		TZ_PROFZONE("scene - update", 0xff0077ee);
		this->rebuild_quadtree();
		this->actors.update(dt,
		{
			.view_bounds = this->cam.get_view_bounds(),
			.level_bounds = {tz::vec2::zero(), tz::vec2::zero()}
		});
		this->handle_camera_zoom();
	}

	void scene::dbgui()
	{
		if(this->actor_dbgui)
		{
			ImGui::Begin("Actor System", &this->actor_dbgui);
			this->actors.dbgui();
			ImGui::End();
		}
		if(this->tree_dbgui)
		{
			ImGui::Begin("Quadtree", &this->tree_dbgui);
			this->tree.dbgui();
			ImGui::End();
		}
	}

	const camera& scene::get_camera() const
	{
		return this->cam;
	}

	const actor_system& scene::get_actor_system() const
	{
		return this->actors;
	}

	std::size_t scene::size() const
	{
		return this->actors.size();
	}

	mount_result scene::mount(std::span<quad_renderer::quad_data> quads)
	{
		mount_result mres;
		mres = this->actors.mount(quads);
		if(mres.error != mount_error::no_error)
		{
			return mres;
		}
		if(this->tree.debug_should_draw())
		{
			mres << this->tree.debug_mount(quads.subspan(mres.count));
		}
		return mres;
	}

	void scene::rebuild_quadtree()
	{
		TZ_PROFZONE("scene - build quadtree", 0xff0077ee);
		this->tree.clear();
		for(const actor& a : this->actors.container())
		{
			if(a.entity.has_component<actor_component_id::collide>())
			{
				box b = a.entity.get_component<actor_component_id::collide>()->data().get_bounding_box(a);
				this->tree.add({.uuid = a.uuid, .bounding_box = b});
			}
		}
		this->actors.set_intersection_state(this->tree.find_all_intersections());
	}

	void scene::handle_camera_zoom()
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
			this->cam.zoom -= 0.1f;
		}
		else if(scroll < scroll_cache)
		{
			this->cam.zoom += 0.1f;
		}
		scroll_cache = scroll;
	}
}