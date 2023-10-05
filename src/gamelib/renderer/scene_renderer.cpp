#include "gamelib/renderer/scene_renderer.hpp"
#include "tz/wsi/monitor.hpp"
#include "tz/lua/api.hpp"
#include <limits>

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
		auto handle = this->entries.back().objects.front();
		// human is way bigger than quad, so cut it down a size a bit.
		if(m == model::humanoid)
		{
			tz::trs trs = tz::ren::animation_renderer::get_object_base_transform(handle);
			trs.scale *= 0.25f;
			tz::ren::animation_renderer::set_object_base_transform(handle, trs);
		}
		return
		{
			.pkg = this->entries.back(),
			.m = m
		};
	}

	scene_element scene_renderer::get_element(entry e)
	{
		return {this, e};
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

	tz::ren::animation_renderer& scene_renderer::get_renderer()
	{
		return *static_cast<tz::ren::animation_renderer*>(this);
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

	scene_renderer::model scene_element::get_model() const
	{
		return this->entry.m;
	}

	std::size_t scene_element::get_animation_count() const
	{
		return this->renderer->get_renderer().get_animation_count(this->entry.pkg);
	}

	std::optional<std::size_t> scene_element::get_playing_animation_id() const
	{
		return this->renderer->get_renderer().get_playing_animation(this->entry.pkg);
	}

	std::string_view scene_element::get_animation_name(std::size_t anim_id) const
	{
		return this->renderer->get_renderer().get_animation_name(this->entry.pkg, anim_id);
	}

	void scene_element::play_animation(std::size_t anim_id, bool loop)
	{
		this->renderer->get_renderer().play_animation(this->entry.pkg, anim_id, loop);
	}

	void scene_element::queue_animation(std::size_t anim_id, bool loop)
	{
		this->renderer->get_renderer().queue_animation(this->entry.pkg, anim_id, loop);
	}

	// LUA API

	struct impl_rn_scene_element
	{
		scene_element elem;
		int get_model(tz::lua::state& state)
		{
			state.stack_push_int(static_cast<int>(this->elem.get_model()));
			return 1;
		}

		int get_animation_count(tz::lua::state& state)
		{
			state.stack_push_uint(this->elem.get_animation_count());
			return 1;
		}

		int get_playing_animation_id(tz::lua::state& state)
		{
			auto maybe_id = this->elem.get_playing_animation_id();
			if(maybe_id.has_value())
			{
				state.stack_push_uint(maybe_id.value());
			}
			else
			{
				state.stack_push_uint(std::numeric_limits<std::uint64_t>::max());
			}
			return 1;
		}

		int is_animation_playing(tz::lua::state& state)
		{
			state.stack_push_bool(this->elem.get_playing_animation_id().has_value());
			return 1;
		}

		int get_animation_name(tz::lua::state& state)
		{
			auto [_, anim_id] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
			state.stack_push_string(this->elem.get_animation_name(anim_id));
			return 1;
		}

		int play_animation(tz::lua::state& state)
		{
			auto [_, anim_id, loop] = tz::lua::parse_args<tz::lua::nil, unsigned int, bool>(state);
			this->elem.play_animation(anim_id, loop);
			return 0;
		}

		int queue_animation(tz::lua::state& state)
		{
			auto [_, anim_id, loop] = tz::lua::parse_args<tz::lua::nil, unsigned int, bool>(state);
			this->elem.queue_animation(anim_id, loop);
			return 0;
		}
	};

	LUA_CLASS_BEGIN(impl_rn_scene_element)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(impl_rn_scene_element, get_model)
			LUA_METHOD(impl_rn_scene_element, get_animation_count)
			LUA_METHOD(impl_rn_scene_element, get_playing_animation_id)
			LUA_METHOD(impl_rn_scene_element, is_animation_playing)
			LUA_METHOD(impl_rn_scene_element, get_animation_name)
			LUA_METHOD(impl_rn_scene_element, play_animation)
			LUA_METHOD(impl_rn_scene_element, queue_animation)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END

	struct impl_rn_scene_renderer
	{
		scene_renderer* renderer = nullptr;
		int add_model(tz::lua::state& state)
		{
			auto [_, modelval] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
			auto mod = static_cast<scene_renderer::model>(modelval);
			LUA_CLASS_PUSH(state, impl_rn_scene_element, {.elem = renderer->get_element(renderer->add_model(mod))});
			return 1;	
		}
	};

	LUA_CLASS_BEGIN(impl_rn_scene_renderer)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(impl_rn_scene_renderer, add_model)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END

	LUA_BEGIN(impl_rn_scene_renderer_get_model_name)
		auto [modelval] = tz::lua::parse_args<int>(state);
		state.stack_push_string(scene_renderer::get_model_name(static_cast<scene_renderer::model>(modelval)));
		return 1;
	LUA_END

	scene_renderer* impl_scene_renderer = nullptr;

	LUA_BEGIN(impl_rn_scene_renderer_get)
		LUA_CLASS_PUSH(state, impl_rn_scene_renderer, {.renderer = impl_scene_renderer});
		return 1;
	LUA_END

	void scene_renderer::lua_initialise(tz::lua::state& state)
	{
		impl_scene_renderer = this;
		state.assign_emptytable("rn");
		state.new_type("impl_rn_scene_element", LUA_CLASS_NAME(impl_rn_scene_element)::registers);
		state.new_type("impl_rn_scene_renderer", LUA_CLASS_NAME(impl_rn_scene_renderer)::registers);
		state.assign_uint("badu", std::numeric_limits<std::uint64_t>::max());
		state.assign_func("rn.get_model_name", LUA_FN_NAME(impl_rn_scene_renderer_get_model_name));
		state.assign_func("rn.get_scene_renderer", LUA_FN_NAME(impl_rn_scene_renderer_get));
	}

}