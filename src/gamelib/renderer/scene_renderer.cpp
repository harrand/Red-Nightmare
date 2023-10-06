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
			this->entries.push_back({.pkg = this->base_models[mid], .m = m});
		}
		else
		{
			tz::ren::animation_renderer::override_package opkg;	
			opkg.overrides = {tz::ren::animation_renderer::override_flag::mesh, tz::ren::animation_renderer::override_flag::texture};
			opkg.pkg = this->base_models[mid];

			this->entries.push_back({.pkg = tz::ren::animation_renderer::add_gltf(scene_renderer::get_model(m), this->root, opkg), .m = m});
		}
		auto handle = this->entries.back().pkg.objects.front();
		// human is way bigger than quad, so cut it down a size a bit.
		if(m == model::humanoid)
		{
			tz::trs trs = tz::ren::animation_renderer::get_object_base_transform(handle);
			trs.scale *= 0.25f;
			tz::ren::animation_renderer::set_object_base_transform(handle, trs);
		}
		return this->entries.back();
	}

	scene_element scene_renderer::get_element(entry e)
	{
		return {this, e};
	}

	scene_renderer::entry scene_renderer::entry_at(std::size_t idx) const
	{
		tz::assert(idx < this->entries.size());
		return this->entries[idx];
	}

	std::size_t scene_renderer::entry_count() const
	{
		return this->entries.size();
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

	std::size_t scene_element::get_object_count() const
	{
		return this->entry.pkg.objects.size();
	}

	tz::ren::texture_locator scene_element::object_get_texture(tz::ren::animation_renderer::object_handle h, std::size_t bound_texture_id) const
	{
		return this->renderer->get_renderer().object_get_texture(h, bound_texture_id);
	}

	void scene_element::object_set_texture(tz::ren::animation_renderer::object_handle h, std::size_t bound_texture_id, tz::ren::texture_locator tloc)
	{
		this->renderer->get_renderer().object_set_texture(h, bound_texture_id, tloc);
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

	void scene_element::skip_animation()
	{
		this->renderer->get_renderer().skip_animation(this->entry.pkg);
	}

	void scene_element::halt_animation()
	{
		this->renderer->get_renderer().halt_animation(this->entry.pkg);
	}

	// LUA API
	int impl_rn_scene_texture_locator::get_colour_tint(tz::lua::state& state)
	{
		state.stack_push_float(this->tloc.colour_tint[0]);
		state.stack_push_float(this->tloc.colour_tint[1]);
		state.stack_push_float(this->tloc.colour_tint[2]);
		return 3;
	}

	int impl_rn_scene_texture_locator::set_colour_tint(tz::lua::state& state)
	{
		auto [_, r, g, b] = tz::lua::parse_args<tz::lua::nil, float, float, float>(state);
		this->tloc.colour_tint = {r, g, b};
		return 0;
	}

	int impl_rn_scene_texture_locator::get_texture_handle(tz::lua::state& state)
	{
		state.stack_push_uint(static_cast<std::size_t>(static_cast<tz::hanval>(this->tloc.texture)));
		return 1;
	}

	int impl_rn_scene_texture_locator::set_texture_handle(tz::lua::state& state)
	{
		auto [_, hanval] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		this->tloc.texture = static_cast<tz::hanval>(hanval);
		return 0;
	}

	int impl_rn_scene_element::get_object_count(tz::lua::state& state)
	{
		state.stack_push_uint(elem.get_object_count());
		return 1;
	}

	int impl_rn_scene_element::object_get_texture(tz::lua::state& state)
	{
		auto [_, oh, bound_texture_id] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int>(state);
		auto objh = this->elem.entry.pkg.objects[oh];
		LUA_CLASS_PUSH(state, impl_rn_scene_texture_locator, {.tloc = this->elem.object_get_texture(objh, bound_texture_id)});
		return 1;
	}

	int impl_rn_scene_element::object_set_texture_tint(tz::lua::state& state)
	{
		auto [_, oh, bound_texture_id, r, g, b] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int, float, float, float>(state);
		auto objh = this->elem.entry.pkg.objects[oh];
		tz::ren::texture_locator tloc = this->elem.object_get_texture(objh, bound_texture_id);
		tloc.colour_tint = {r, g, b};
		this->elem.object_set_texture(objh, bound_texture_id, tloc);
		return 0;
	}

	int impl_rn_scene_element::object_set_texture_handle(tz::lua::state& state)
	{
		auto [_, oh, bound_texture_id, texhandle] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int, unsigned int>(state);
		auto objh = this->elem.entry.pkg.objects[oh];
		tz::ren::texture_locator tloc = this->elem.object_get_texture(objh, bound_texture_id);
		tloc.texture = static_cast<tz::hanval>(texhandle);
		this->elem.object_set_texture(objh, bound_texture_id, tloc);
		return 0;
	}

	int impl_rn_scene_element::face_forward(tz::lua::state& state)
	{
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		auto transform = ren.get_object_base_transform(objh);
		transform.rotate = tz::quat::from_axis_angle(tz::vec3::zero(), 1.0f);
		ren.set_object_base_transform(objh, transform);
		return 0;
	}

	int impl_rn_scene_element::face_left(tz::lua::state& state)
	{
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		transform.rotate = tz::quat::from_axis_angle({0.0f, 1.0f, 0.0f}, -1.5708f);
		ren.set_object_base_transform(objh, transform);
		return 0;
	}

	int impl_rn_scene_element::face_right(tz::lua::state& state)
	{
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		transform.rotate = tz::quat::from_axis_angle({0.0f, 1.0f, 0.0f}, 1.5708f);
		ren.set_object_base_transform(objh, transform);
		return 0;
	}

	int impl_rn_scene_element::get_position(tz::lua::state& state)
	{
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		state.stack_push_float(transform.translate[0]);
		state.stack_push_float(transform.translate[1]);
		return 2;
	}

	int impl_rn_scene_element::set_position(tz::lua::state& state)
	{
		auto [_, x, y] = tz::lua::parse_args<tz::lua::nil, float, float>(state);
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		transform.translate[0] = x;
		transform.translate[1] = y;
		ren.set_object_base_transform(objh, transform);
		return 0;
	}

	int impl_rn_scene_element::get_model(tz::lua::state& state)
	{
		state.stack_push_int(static_cast<int>(this->elem.get_model()));
		return 1;
	}

	int impl_rn_scene_element::get_animation_count(tz::lua::state& state)
	{
		state.stack_push_uint(this->elem.get_animation_count());
		return 1;
	}

	int impl_rn_scene_element::get_playing_animation_id(tz::lua::state& state)
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

	int impl_rn_scene_element::is_animation_playing(tz::lua::state& state)
	{
		bool playing = this->elem.get_playing_animation_id().has_value();
		if(playing)
		{
			const float progress = this->elem.renderer->get_renderer().get_playing_animation_progress(this->elem.entry.pkg);
			playing &= (progress < 1.0f);
		}
		state.stack_push_bool(playing);
		return 1;
	}

	int impl_rn_scene_element::get_animation_name(tz::lua::state& state)
	{
		auto [_, anim_id] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		state.stack_push_string(this->elem.get_animation_name(anim_id));
		return 1;
	}

	int impl_rn_scene_element::play_animation(tz::lua::state& state)
	{
		auto [_, anim_id, loop] = tz::lua::parse_args<tz::lua::nil, unsigned int, bool>(state);
		this->elem.play_animation(anim_id, loop);
		return 0;
	}

	int impl_rn_scene_element::queue_animation(tz::lua::state& state)
	{
		auto [_, anim_id, loop] = tz::lua::parse_args<tz::lua::nil, unsigned int, bool>(state);
		this->elem.queue_animation(anim_id, loop);
		return 0;
	}

	int impl_rn_scene_element::skip_animation(tz::lua::state& state)
	{
		this->elem.skip_animation();
		return 0;
	}

	int impl_rn_scene_element::halt_animation(tz::lua::state& state)
	{
		this->elem.halt_animation();
		return 0;
	}

	int impl_rn_scene_renderer::add_model(tz::lua::state& state)
	{
		auto [_, modelval] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		auto mod = static_cast<scene_renderer::model>(modelval);
		LUA_CLASS_PUSH(state, impl_rn_scene_element, {.elem = renderer->get_element(renderer->add_model(mod))});
		return 1;	
	}

	int impl_rn_scene_renderer::get_element(tz::lua::state& state)
	{
		auto [_, entry_id] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		LUA_CLASS_PUSH(state, impl_rn_scene_element, {.elem = this->renderer->get_element(this->renderer->entry_at(entry_id))});
		return 1;
	}

	int impl_rn_scene_renderer::element_count(tz::lua::state& state)
	{
		state.stack_push_uint(this->renderer->entry_count());
		return 1;
	}

	int impl_rn_scene_renderer::load_texture_from_disk(tz::lua::state& state)
	{
		auto [_, path] = tz::lua::parse_args<tz::lua::nil, std::string>(state);
		tz::ren::animation_renderer::texture_handle rethan = this->renderer->get_renderer().add_texture(tz::io::image::load_from_file(path));
		state.stack_push_uint(static_cast<std::size_t>(static_cast<tz::hanval>(rethan)));
		return 1;
	}

	LUA_BEGIN(impl_rn_scene_renderer_get_model_name)
		auto [modelval] = tz::lua::parse_args<int>(state);
		state.stack_push_string(scene_renderer::get_model_name(static_cast<scene_renderer::model>(modelval)));
		return 1;
	LUA_END

	void scene_renderer::lua_initialise(tz::lua::state& state)
	{
		state.new_type("impl_rn_scene_texture_locator", LUA_CLASS_NAME(impl_rn_scene_texture_locator)::registers);
		state.new_type("impl_rn_scene_element", LUA_CLASS_NAME(impl_rn_scene_element)::registers);
		state.new_type("impl_rn_scene_renderer", LUA_CLASS_NAME(impl_rn_scene_renderer)::registers);
		state.assign_uint("badu", std::numeric_limits<std::uint64_t>::max());
		state.assign_func("rn.get_model_name", LUA_FN_NAME(impl_rn_scene_renderer_get_model_name));
	}

}