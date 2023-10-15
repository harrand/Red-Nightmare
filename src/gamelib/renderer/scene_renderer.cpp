#include "gamelib/renderer/scene_renderer.hpp"
#include "tz/core/profile.hpp"
#include "tz/ren/animation.hpp"
#include "tz/wsi/monitor.hpp"
#include "tz/gl/resource.hpp"
#include "tz/lua/api.hpp"
#include <limits>

#include "tz/gl/imported_shaders.hpp"
#include ImportedShaderHeader(pixelate, vertex)
#include ImportedShaderHeader(pixelate, fragment)
#include ImportedShaderHeader(scene_renderer, fragment)

namespace game::render
{
	scene_renderer::scene_renderer():
	pixelate_pass(),
	output
	{tz::gl::image_output_info{
		.colours =
		{
			this->pixelate_pass.get_background_image(),
			this->pixelate_pass.get_foreground_image()
		}
	}},
	renderer
	(
		128u,
		{tz::gl::renderer_option::no_present},
		ImportedShaderSource(scene_renderer, fragment),
		&this->output
	)
	{
		TZ_PROFZONE("scene renderer - create", 0xFFFF4488);
		this->renderer.append_to_render_graph();
		// add pixelate pass to render graph, and then make sure it depends on animation renderer.
		tz::gl::get_device().render_graph().timeline.push_back(static_cast<tz::gl::eid_t>(static_cast<tz::hanval>(this->pixelate_pass.handle)));
		tz::gl::get_device().render_graph().add_dependencies(this->pixelate_pass.handle, this->renderer.get_render_pass());
		this->root = this->renderer.add_object
		({
			.mesh = tz::nullhand,
			.bound_textures = {},
			.parent = tz::nullhand,
		});
	}

	scene_renderer::entry scene_renderer::add_model(model m)
	{
		TZ_PROFZONE("scene renderer - add model", 0xFFFF4488);
		auto mid = static_cast<int>(m);	
		if(this->base_models[mid].objects.empty())
		{
			// no base model exists, create a new one.
			this->base_models[mid] = this->renderer.add_gltf(scene_renderer::get_model(m), this->root);
			this->entries.push_back({.pkg = this->base_models[mid], .m = m});
		}
		else
		{
			tz::ren::animation_renderer::override_package opkg;	
			opkg.overrides = {tz::ren::animation_renderer::override_flag::mesh, tz::ren::animation_renderer::override_flag::texture};
			opkg.pkg = this->base_models[mid];

			this->entries.push_back({.pkg = this->renderer.add_gltf(scene_renderer::get_model(m), this->root, opkg), .m = m});
		}
		auto handle = this->entries.back().pkg.objects.front();
		// human is way bigger than quad, so cut it down a size a bit.
		if(m == model::humanoid)
		{
			tz::trs trs = this->renderer.get_object_base_transform(handle);
			trs.scale *= 0.25f;
			this->renderer.set_object_base_transform(handle, trs);
		}
		return this->entries.back();
	}

	void scene_renderer::remove_model(entry e)
	{
		if(this->base_models[static_cast<int>(e.m)] == e.pkg)
		{
			// this is the base model. gotta remove it somehow :(
			// we could set base model to empty here, but it means the next time this model is added, the mesh data is added again
			// so we cannot do anything until we delete the actual meshes and textures.
		}
		this->renderer.remove_objects(e.pkg, tz::ren::animation_renderer::transform_hierarchy::remove_strategy::remove_children);
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
		TZ_PROFZONE("scene renderer - update", 0xFFFF4488);
		this->update_camera(delta);
		this->renderer.update(delta);
		const tz::vec2ui mondims = tz::wsi::get_monitors().front().dimensions;
		const float aspect_ratio = static_cast<float>(mondims[0]) / mondims[1];
		this->renderer.camera_orthographic
		({
			.left = -this->view_bounds[0],
			.right = this->view_bounds[0],
			.top = this->view_bounds[1] / aspect_ratio,
			.bottom = -this->view_bounds[1] / aspect_ratio,
			.near_plane = -20.0f,
			.far_plane = 21.5f
		});

		this->pixelate_pass.handle_resize(this->renderer.get_render_pass());
	}

	void scene_renderer::dbgui()
	{
		this->renderer.dbgui();
	}

	tz::ren::animation_renderer& scene_renderer::get_renderer()
	{
		return this->renderer;
	}

	void scene_renderer::update_camera(float delta)
	{
		TZ_PROFZONE("scene renderer - update camera", 0xFFFF4488);
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
			const float multiplier = (1.0f - std::clamp(delta, 0.1f, 0.3f));
			this->view_bounds *= multiplier;
			this->pixelate_pass.zoom_amount() *= multiplier;
		}
		else if(ynow < ybefore)
		{
			// scrolled down.	
			const float multiplier = (1.0f + std::clamp(delta, 0.1f, 0.3f));
			this->view_bounds *= multiplier;
			this->pixelate_pass.zoom_amount() *= multiplier;
		}
	}

	// pixelate pass
	scene_renderer::pixelate_pass_t::pixelate_pass_t()
	{
		// todo: we depend on animation renderer's render pass.
		tz::gl::renderer_info rinfo;
		rinfo.state().graphics.tri_count = 1;
		rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(pixelate, vertex));
		rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(pixelate, fragment));
		//auto mondims = tz::wsi::get_monitors().front().dimensions;
		auto mondims = tz::window().get_dimensions();
		std::array<float, 2> dimension_buffer_data;
		dimension_buffer_data[0] = mondims[0];
		dimension_buffer_data[1] = mondims[1];
		this->zoom_buffer = rinfo.add_resource(tz::gl::buffer_resource::from_one(1.0f,
		{
			.access = tz::gl::resource_access::dynamic_access
		}));
		this->dimension_buffer = rinfo.add_resource(tz::gl::buffer_resource::from_many(dimension_buffer_data));
		this->bg_image = rinfo.add_resource(tz::gl::image_resource::from_uninitialised
		({
			.format = tz::gl::image_format::BGRA32,
			.dimensions = mondims,
			.flags = {tz::gl::resource_flag::renderer_output}
		}));
		this->fg_image = rinfo.add_resource(tz::gl::image_resource::from_uninitialised
		({
			.format = tz::gl::image_format::BGRA32,
			.dimensions = mondims,
			.flags = {tz::gl::resource_flag::renderer_output}
		}));
		this->handle = tz::gl::get_device().create_renderer(rinfo);
		this->dims_cache = tz::window().get_dimensions();
	}

	tz::gl::icomponent* scene_renderer::pixelate_pass_t::get_background_image()
	{
		return tz::gl::get_device().get_renderer(this->handle).get_component(this->bg_image);
	}

	tz::gl::icomponent* scene_renderer::pixelate_pass_t::get_foreground_image()
	{
		return tz::gl::get_device().get_renderer(this->handle).get_component(this->fg_image);
	}

	float& scene_renderer::pixelate_pass_t::zoom_amount()
	{
		return tz::gl::get_device().get_renderer(this->handle).get_resource(this->zoom_buffer)->data_as<float>().front();
	}

	void scene_renderer::pixelate_pass_t::handle_resize(tz::gl::renderer_handle animation_render_pass)
	{
		if(tz::window().get_dimensions() != this->dims_cache && tz::window().get_dimensions() != tz::vec2ui{0u, 0u})
		{
			// we have been resized.
			// firstly resize our foreground and background images.
			{
				tz::gl::RendererEditBuilder builder;
				auto windims = static_cast<tz::vec2>(tz::window().get_dimensions());
				builder.write
				({
					.resource = this->dimension_buffer,
					.data = std::as_bytes(windims.data()),
					.offset = 0
				});
				builder.image_resize
				({
					.image_handle = this->bg_image,
					.dimensions = tz::window().get_dimensions()
				});
				builder.image_resize
				({
					.image_handle = this->fg_image,
					.dimensions = tz::window().get_dimensions()
				});
				builder.mark_dirty({.images = true});
				tz::gl::get_device().get_renderer(this->handle).edit(builder.build());
			}

			// then tell the animation renderer to recreate its render targets.
			{
				tz::gl::RendererEditBuilder builder;
				builder.mark_dirty
				({
					.work_commands = true,
					.render_targets = true,
				});
				tz::gl::get_device().get_renderer(animation_render_pass).edit(builder.build());
				this->dims_cache = tz::window().get_dimensions();
			}
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

	bool scene_element::object_get_visibility(tz::ren::animation_renderer::object_handle h) const
	{
		return this->renderer->get_renderer().object_get_visible(h);
	}

	void scene_element::object_set_visibility(tz::ren::animation_renderer::object_handle h, bool visible)
	{
		this->renderer->get_renderer().object_set_visible(h, visible);
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
		TZ_PROFZONE("scene element - object set texture tint", 0xFFFFAAEE);
		auto [_, oh, bound_texture_id, r, g, b] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int, float, float, float>(state);
		auto objh = this->elem.entry.pkg.objects[oh];
		tz::ren::texture_locator tloc = this->elem.object_get_texture(objh, bound_texture_id);
		tloc.colour_tint = {r, g, b};
		this->elem.object_set_texture(objh, bound_texture_id, tloc);
		return 0;
	}

	int impl_rn_scene_element::object_set_texture_handle(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - object set texture handle", 0xFFFFAAEE);
		auto [_, oh, bound_texture_id, texhandle] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int, unsigned int>(state);
		auto objh = this->elem.entry.pkg.objects[oh];
		tz::ren::texture_locator tloc = this->elem.object_get_texture(objh, bound_texture_id);
		tloc.texture = static_cast<tz::hanval>(texhandle);
		this->elem.object_set_texture(objh, bound_texture_id, tloc);
		return 0;
	}

	int impl_rn_scene_element::object_get_colour_tint(tz::lua::state& state)
	{
		auto [_, oh] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		auto objh = this->elem.entry.pkg.objects[oh];
		tz::vec3 ret = this->elem.renderer->get_renderer().object_get_colour(objh);
		state.stack_push_float(ret[0]);
		state.stack_push_float(ret[1]);
		state.stack_push_float(ret[2]);
		return 3;
	}

	int impl_rn_scene_element::object_set_colour_tint(tz::lua::state& state)
	{
		auto [_, oh, r, g, b] = tz::lua::parse_args<tz::lua::nil, unsigned int, float, float, float>(state);
		auto objh = this->elem.entry.pkg.objects[oh];
		this->elem.renderer->get_renderer().object_set_colour(objh, {r, g, b});
		return 0;
	}
	
	int impl_rn_scene_element::object_get_visibility(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - object get visibility", 0xFFFFAAEE);
		auto [_, oh] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		auto objh = this->elem.entry.pkg.objects[oh];
		state.stack_push_bool(this->elem.object_get_visibility(objh));
		return 1;
	}

	int impl_rn_scene_element::object_set_visibility(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - object set visibility", 0xFFFFAAEE);
		auto [_, oh, visible] = tz::lua::parse_args<tz::lua::nil, unsigned int, bool>(state);
		auto objh = this->elem.entry.pkg.objects[oh];
		this->elem.object_set_visibility(objh, visible);
		return 0;
	}

	int impl_rn_scene_element::face_forward(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - face forward", 0xFFFFAAEE);
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		auto transform = ren.get_object_base_transform(objh);
		transform.rotate = tz::quat::from_axis_angle({1.0f, 0.0f, 0.0f}, 0.3f);
		ren.set_object_base_transform(objh, transform);
		return 0;
	}

	int impl_rn_scene_element::face_backward(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - face backward", 0xFFFFAAEE);
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		auto transform = ren.get_object_base_transform(objh);
		transform.rotate = tz::quat::from_axis_angle({0.0f, 1.0f, 0.2f}, 3.14159f);
		ren.set_object_base_transform(objh, transform);
		return 0;
	}


	int impl_rn_scene_element::face_left(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - face left", 0xFFFFAAEE);
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		transform.rotate = tz::quat::from_axis_angle({-0.2f, 1.0f, 0.2f}, -1.5708f);
		ren.set_object_base_transform(objh, transform);
		return 0;
	}

	int impl_rn_scene_element::face_right(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - face right", 0xFFFFAAEE);
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		transform.rotate = tz::quat::from_axis_angle({0.2f, 1.0f, 0.2f}, 1.5708f);
		ren.set_object_base_transform(objh, transform);
		return 0;
	}

	int impl_rn_scene_element::rotate(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - rotate", 0xFFFFAAEE);
		auto [_, angle] = tz::lua::parse_args<tz::lua::nil, float>(state);
		auto objh  = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		transform.rotate.combine(tz::quat::from_axis_angle({0.0f, 0.0f, 1.0f}, angle));
		ren.set_object_base_transform(objh, transform);
		return 0;
	}

	int impl_rn_scene_element::get_position(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - get position", 0xFFFFAAEE);
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		state.stack_push_float(transform.translate[0]);
		state.stack_push_float(transform.translate[1]);
		return 2;
	}

	int impl_rn_scene_element::set_position(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - set position", 0xFFFFAAEE);
		auto [_, x, y] = tz::lua::parse_args<tz::lua::nil, float, float>(state);
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		transform.translate[0] = x;
		transform.translate[1] = y;
		ren.set_object_base_transform(objh, transform);
		return 0;
	}

	int impl_rn_scene_element::get_subobject_position(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - get position", 0xFFFFAAEE);
		auto [_, oid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		auto objh = this->elem.entry.pkg.objects[oid];
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_global_transform(objh);
		state.stack_push_float(transform.translate[0]);
		state.stack_push_float(transform.translate[1]);
		return 2;
	}

	int impl_rn_scene_element::get_scale(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - get scale", 0xFFFFAAEE);
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		state.stack_push_float(transform.scale[0]);
		state.stack_push_float(transform.scale[1]);
		return 2;
	}

	int impl_rn_scene_element::set_scale(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - set scale", 0xFFFFAAEE);
		auto [_, x, y] = tz::lua::parse_args<tz::lua::nil, float, float>(state);
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		transform.scale[0] = x;
		transform.scale[1] = y;
		ren.set_object_base_transform(objh, transform);
		return 0;
	}

	int impl_rn_scene_element::get_uniform_scale(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - get uniform scale", 0xFFFFAAEE);
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		tz::assert(transform.scale[0] == transform.scale[1] && transform.scale[1] && transform.scale[2]);
		state.stack_push_float(transform.scale[0]);
		return 1;
	}

	int impl_rn_scene_element::set_uniform_scale(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - set uniform scale", 0xFFFFAAEE);
		auto [_, s] = tz::lua::parse_args<tz::lua::nil, float>(state);
		auto objh = this->elem.entry.pkg.objects.front();
		auto& ren = this->elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(objh);
		transform.scale = tz::vec3::filled(s);
		ren.set_object_base_transform(objh, transform);
		return 0;
	}

	int impl_rn_scene_element::get_model(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - get model", 0xFFFFAAEE);
		state.stack_push_int(static_cast<int>(this->elem.get_model()));
		return 1;
	}

	int impl_rn_scene_element::get_animation_count(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - get animation count", 0xFFFFAAEE);
		state.stack_push_uint(this->elem.get_animation_count());
		return 1;
	}

	int impl_rn_scene_element::get_playing_animation_id(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - get animation", 0xFFFFAAEE);
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
		TZ_PROFZONE("scene element - is animation playing", 0xFFFFAAEE);
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
		TZ_PROFZONE("scene element - get animation name", 0xFFFFAAEE);
		auto [_, anim_id] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		state.stack_push_string(this->elem.get_animation_name(anim_id));
		return 1;
	}

	int impl_rn_scene_element::get_animation_speed(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - get animation speed", 0xFFFFAAEE);
		state.stack_push_float(this->elem.renderer->get_renderer().get_animation_speed(this->elem.entry.pkg));
		return 1;
	}

	int impl_rn_scene_element::set_animation_speed(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - set animation speed", 0xFFFFAAEE);
		auto [_, anim_speed] = tz::lua::parse_args<tz::lua::nil, float>(state);
		this->elem.renderer->get_renderer().set_animation_speed(this->elem.entry.pkg, anim_speed);
		return 0;
	}

	int impl_rn_scene_element::play_animation(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - play animation", 0xFFFFAAEE);
		auto [_, anim_id, loop] = tz::lua::parse_args<tz::lua::nil, unsigned int, bool>(state);
		this->elem.play_animation(anim_id, loop);
		return 0;
	}

	int impl_rn_scene_element::queue_animation(tz::lua::state& state)
	{
		TZ_PROFZONE("scene element - queue animation", 0xFFFFAAEE);
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
		TZ_PROFZONE("scene element - add model", 0xFFFFAAEE);
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
		TZ_PROFZONE("scene element - load texture from disk", 0xFFFFAAEE);
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
		TZ_PROFZONE("scene renderer - lua initialise", 0xFFFF4488);
		state.new_type("impl_rn_scene_texture_locator", LUA_CLASS_NAME(impl_rn_scene_texture_locator)::registers);
		state.new_type("impl_rn_scene_element", LUA_CLASS_NAME(impl_rn_scene_element)::registers);
		state.new_type("impl_rn_scene_renderer", LUA_CLASS_NAME(impl_rn_scene_renderer)::registers);
		state.assign_uint("badu", std::numeric_limits<std::uint64_t>::max());
		state.assign_func("rn.get_model_name", LUA_FN_NAME(impl_rn_scene_renderer_get_model_name));
	}

}