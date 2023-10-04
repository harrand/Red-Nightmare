#include "gamelib/renderer/scene_renderer.hpp"
#include "tz/wsi/monitor.hpp"

namespace game::render
{
	scene_renderer::scene_renderer()
	{
		tz::ren::animation_renderer::append_to_render_graph();
		this->root = tz::ren::animation_renderer::add_object
		({
			.trs = 
			{
				.rotate = tz::quat::from_axis_angle({0.0f, 1.0f, 0.0f}, 1.5708f)
			},
			.mesh = tz::nullhand,
			.bound_textures = {},
			.parent = tz::nullhand,
		});
	}

	scene_renderer::entry scene_renderer::add_model(model m)
	{
		auto mid = static_cast<int>(m);	
		if(this->base_models[mid].objects.empty())
		{
			// no base model exists, create a new one.
			this->base_models[mid] = tz::ren::animation_renderer::add_gltf(scene_renderer::get_model(m), this->root);
			this->entries.push_back(this->base_models[mid]);
		}
		else
		{
			tz::ren::animation_renderer::override_package opkg;	
			opkg.overrides = {tz::ren::animation_renderer::override_flag::mesh, tz::ren::animation_renderer::override_flag::texture};
			opkg.pkg = this->base_models[mid];

			this->entries.push_back(tz::ren::animation_renderer::add_gltf(scene_renderer::get_model(m), this->root, opkg));
		}
		auto handle = static_cast<tz::hanval>(this->entries.size());
		// human is way bigger than quad, so cut it down a size a bit.
		if(m == model::humanoid)
		{
			tz::trs trs = tz::ren::animation_renderer::get_object_base_transform(this->entries.back().objects.front());
			trs.scale *= 0.25f;
			tz::ren::animation_renderer::set_object_base_transform(this->entries.back().objects.front(), trs);
		}
		return
		{
			.handle = handle,
			.m = m
		};
	}

	void scene_renderer::update(float delta)
	{
		this->update_camera(delta);
		tz::ren::animation_renderer::update(delta);
		const tz::vec2ui mondims = tz::wsi::get_monitors().front().dimensions;
		const float aspect_ratio = static_cast<float>(mondims[0]) / mondims[1];
		tz::ren::animation_renderer::camera_orthographic
		({
			.left = -this->view_bounds[0],
			.right = this->view_bounds[0],
			.top = this->view_bounds[1] / aspect_ratio,
			.bottom = -this->view_bounds[1] / aspect_ratio,
			.near_plane = -20.0f,
			.far_plane = 21.5f
		});
	}

	void scene_renderer::dbgui()
	{
		tz::ren::animation_renderer::dbgui();
	}

	void scene_renderer::update_camera(float delta)
	{
		if(tz::dbgui::claims_mouse())
		{
			return;
		}
		int ybefore = this->impl_mouse_scroll_delta;
		int ynow = tz::window().get_mouse_state().wheel_position;
		this->impl_mouse_scroll_delta = ynow;
		if(ynow > ybefore)
		{
			// scrolled up.
			this->view_bounds *= (1.0f - std::clamp(delta, 0.1f, 0.3f));
		}
		else if(ynow < ybefore)
		{
			// scrolled down.	
			this->view_bounds *= (1.0f + std::clamp(delta, 0.1f, 0.3f));
		}
	}
}