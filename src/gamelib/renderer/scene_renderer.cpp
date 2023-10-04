#include "gamelib/renderer/scene_renderer.hpp"

namespace game::render
{
	scene_renderer::scene_renderer()
	{
		tz::ren::animation_renderer::append_to_render_graph();
	}

	scene_renderer::entry scene_renderer::add_model(model m)
	{
		auto mid = static_cast<int>(m);	
		if(this->base_models[mid].objects.empty())
		{
			// no base model exists, create a new one.
			this->base_models[mid] = tz::ren::animation_renderer::add_gltf(scene_renderer::get_model(m));
			this->entries.push_back(this->base_models[mid]);
		}
		else
		{
			tz::ren::animation_renderer::override_package opkg;	
			opkg.overrides = {tz::ren::animation_renderer::override_flag::mesh, tz::ren::animation_renderer::override_flag::texture};
			opkg.pkg = this->base_models[mid];

			this->entries.push_back(tz::ren::animation_renderer::add_gltf(scene_renderer::get_model(m), opkg));
		}
		return
		{
			.handle = static_cast<tz::hanval>(this->entries.size()),
			.m = m
		};
	}

	void scene_renderer::update(float delta)
	{
		tz::ren::animation_renderer::update(delta);
	}

	void scene_renderer::dbgui()
	{
		tz::ren::animation_renderer::dbgui();
	}
}