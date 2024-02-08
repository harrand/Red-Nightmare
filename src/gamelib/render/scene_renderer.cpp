#include "gamelib/render/scene_renderer.hpp"
#include "gamelib/messaging/scene.hpp"
#include "tz/core/profile.hpp"
#include "tz/wsi/monitor.hpp"
#include "tz/gl/resource.hpp"
#include "tz/lua/api.hpp"
#include <limits>

#include "tz/gl/imported_shaders.hpp"
#include ImportedShaderHeader(pixelate, vertex)
#include ImportedShaderHeader(pixelate, fragment)
#include ImportedShaderHeader(scene_renderer, fragment)

#include ImportedTextHeader(ProggyClean, ttf)

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
	({
		.custom_fragment_spirv = ImportedShaderSource(scene_renderer, fragment),
		.custom_options = {tz::gl::renderer_option::no_present, tz::gl::renderer_option::alpha_blending},
		.texture_capacity = 128u,
		.extra_buffers = evaluate_extra_buffers(),
		.output = &this->output,
	}),
	text_renderer(1024u, tz::gl::renderer_options{tz::gl::renderer_option::no_clear_output, tz::gl::renderer_option::no_depth_testing})
	{
		TZ_PROFZONE("scene renderer - create", 0xFFFF4488);
		this->renderer.append_to_render_graph();
		// add pixelate pass to render graph, and then make sure it depends on animation renderer.
		tz::gl::get_device().render_graph().timeline.push_back(static_cast<tz::gl::eid_t>(static_cast<tz::hanval>(this->pixelate_pass.handle)));

		this->text_renderer.append_to_render_graph();

		auto fh = this->text_renderer.add_font(tz::io::ttf::from_memory(ImportedTextData(ProggyClean, ttf)));
		this->text_renderer.add_string(fh, {.scale = tz::vec3::filled(20.0f)}, "test");

		tz::gl::get_device().render_graph().add_dependencies(this->pixelate_pass.handle, this->renderer.get_render_pass());
		tz::gl::get_device().render_graph().add_dependencies(this->text_renderer.get_render_pass(), this->pixelate_pass.handle);
		this->root = this->renderer.add_object
		({
			.mesh = tz::nullhand,
			.parent = tz::nullhand,
			.bound_textures = {},
		});
		
		//for(int i = 0; i < static_cast<int>(model::_count); i++)
		//{
		//	this->renderer.add_gltf(this->get_model(static_cast<model>(i)));
		//}

		constexpr float initial_zoom = 4.0f;
		this->view_bounds /= initial_zoom;
		this->pixelate_pass.zoom_amount() /= initial_zoom;
	}

	void scene_renderer::add_model(std::string model_name, tz::io::gltf model)
	{
		if(this->registered_models.contains(model_name))
		{
			tz::report("Warning: Attempted to register model \"%s\", but a model was already registered with that name", model_name.c_str());
		}
		this->registered_models[model_name] = this->renderer.add_gltf(model);
	}

	void scene_renderer::remove_model(std::string model_name)
	{
		this->registered_models.erase(model_name);
		// TODO: remove from underlying renderer?
	}

	void scene_renderer::add_texture(std::string texture_name, tz::io::image image)
	{
		if(this->registered_textures.contains(texture_name))
		{
			tz::report("Warning: Attempted to register texture \"%s\", but a texture was already registered with that name", texture_name.c_str());
		}
		this->registered_textures[texture_name] = this->renderer.add_texture(image);
	}

	void scene_renderer::remove_texture(std::string texture_name)
	{
		this->registered_textures.erase(texture_name);
		// TODO: remove from underlying renderer?
	}

	tz::ren::animation_renderer::texture_handle scene_renderer::get_texture(std::string texture_name) const
	{
		auto iter = this->registered_textures.find(texture_name);
		if(iter == this->registered_textures.end())
		{
			return tz::nullhand;
		}
		return iter->second;
	}

	std::string scene_renderer::get_texture_name(tz::ren::animation_renderer::texture_handle texh) const
	{
		if(texh == tz::nullhand)
		{
			return "Null Texture";
		}
		// very slow. boo
		for(const auto& [name, handle] : this->registered_textures)
		{
			if(handle == texh)
			{
				return name;
			}
		}
		return "<missingtex>";
	}

	scene_renderer::entry scene_renderer::add_entry(std::string model_name)
	{
		TZ_PROFZONE("scene renderer - add model", 0xFFFF4488);
		auto model_iter = this->registered_models.find(model_name);
		tz::assert(model_iter != this->registered_models.end(), "No model \"%s\" exists. Did you forget to register it?", model_name.c_str());
		tz::ren::animation_renderer::animated_objects_handle aoh = this->renderer.add_animated_objects
		({
			.gltf = model_iter->second,
			.parent = this->root
		});
		this->entries.push_back({.obj = aoh, .model_name = model_name});
		// human is way bigger than quad, so cut it down a size a bit.
		/*
		if(m == model::humanoid)
		{
			tz::trs trs = this->renderer.animated_object_get_local_transform(aoh);
			trs.scale *= 0.25f;
			this->renderer.animated_object_set_local_transform(aoh, trs);
			for(tz::ren::animation_renderer::object_handle oh : this->renderer.animated_object_get_subobjects(aoh))
			{
				// mark all these objects as pixelated.
				this->renderer.get_object(oh).unused2[2] = true;
			}
		}
		*/
		return this->entries.back();
	}

	void scene_renderer::remove_entry(entry e)
	{
		TZ_PROFZONE("scene renderer - remove entry", 0xFFFF4488);
		this->renderer.remove_animated_objects(e.obj);
		// todo: remove or set-null the given entry? otherwise you can remove twice?
	}

	void scene_renderer::clear_entries()
	{
		TZ_PROFZONE("scene renderer - clear entries", 0xFFFF4488);
		for(entry e : this->entries)
		{
			this->renderer.remove_animated_objects(e.obj);
		}
		this->entries.clear();
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

	const tz::ren::text_renderer& scene_renderer::get_text_renderer() const
	{
		return this->text_renderer;
	}

	tz::ren::text_renderer& scene_renderer::get_text_renderer()
	{
		return this->text_renderer;
	}

	const tz::vec2& scene_renderer::get_view_bounds() const
	{
		return this->view_bounds;
	}

	tz::vec2 scene_renderer::get_camera_position() const
	{
		return this->renderer.get_camera_transform().translate.swizzle<0, 1>();
	}

	void scene_renderer::set_camera_position(tz::vec2 cam_pos)
	{
		tz::trs trs = this->renderer.get_camera_transform();
		trs.translate[0] = cam_pos[0];
		trs.translate[1] = cam_pos[1];
		this->renderer.set_camera_transform(trs);
	}

	void scene_renderer::update(float delta)
	{
		TZ_PROFZONE("scene renderer - update", 0xFFFF4488);
		this->update_camera(delta);
		this->renderer.update(delta);
		this->text_renderer.update();
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

	void scene_renderer::block()
	{
		this->renderer.block();
	}

	void scene_renderer::dbgui()
	{
		if(ImGui::BeginTabBar("animation-renderer"))
		{
			this->renderer.dbgui();
			ImGui::EndTabBar();
		}
	}

	tz::vec3 scene_renderer::get_ambient_light() const
	{
		tz::gl::resource_handle light_buf_handle = this->renderer.get_extra_buffer(1);
		std::span<const std::byte> bufdata = tz::gl::get_device().get_renderer(this->renderer.get_render_pass()).get_resource(light_buf_handle)->data();
		return *reinterpret_cast<const tz::vec3*>(bufdata.data());
	}

	void scene_renderer::set_ambient_light(tz::vec3 rgb_light)
	{
		tz::gl::resource_handle light_buf_handle = this->renderer.get_extra_buffer(1);
		std::span<std::byte> bufdata = tz::gl::get_device().get_renderer(this->renderer.get_render_pass()).get_resource(light_buf_handle)->data();
		*reinterpret_cast<tz::vec3*>(bufdata.data()) = rgb_light;
	}

	std::span<const scene_renderer::point_light_data> scene_renderer::get_point_lights() const
	{
		tz::gl::resource_handle light_buf_handle = this->renderer.get_extra_buffer(1);
		std::span<const std::byte> bufdata = tz::gl::get_device().get_renderer(this->renderer.get_render_pass()).get_resource(light_buf_handle)->data();
		std::uint32_t point_light_count = *reinterpret_cast<const std::uint32_t*>(bufdata.data() + sizeof(tz::vec3));
		const auto* point_lights_start = reinterpret_cast<const point_light_data*>(bufdata.data() + sizeof(tz::vec3) + sizeof(std::uint32_t));
		return {point_lights_start, point_light_count};
	}

	std::span<scene_renderer::point_light_data> scene_renderer::get_point_lights()
	{
		tz::gl::resource_handle light_buf_handle = this->renderer.get_extra_buffer(1);
		std::span<std::byte> bufdata = tz::gl::get_device().get_renderer(this->renderer.get_render_pass()).get_resource(light_buf_handle)->data();
		std::uint32_t point_light_count = *reinterpret_cast<const std::uint32_t*>(bufdata.data() + sizeof(tz::vec3));
		auto* point_lights_start = reinterpret_cast<point_light_data*>(bufdata.data() + sizeof(tz::vec3) + sizeof(std::uint32_t));
		return {point_lights_start, point_light_count};
	}

	void scene_renderer::set_point_light_capacity(unsigned int num_point_lights)
	{
		tz::gl::resource_handle light_buf_handle = this->renderer.get_extra_buffer(1);
		std::size_t old_size = sizeof(tz::vec3) + sizeof(std::uint32_t) + sizeof(point_light_data) * this->get_point_lights().size();
		std::size_t new_size = sizeof(tz::vec3) + sizeof(std::uint32_t) + sizeof(point_light_data) * num_point_lights;
		if(old_size == new_size)
		{
			return;
		}
		auto& ren = tz::gl::get_device().get_renderer(this->renderer.get_render_pass());
		// resize buffer
		ren.edit
		({
			tz::gl::RendererEditBuilder{}
			.buffer_resize
			({
				.buffer_handle = light_buf_handle,
				.size = new_size
			})
			.build()
		});
		// write new count.
		// we're sorted.
		*reinterpret_cast<std::uint32_t*>(ren.get_resource(light_buf_handle)->data().data() + sizeof(tz::vec3)) = num_point_lights;
	}

	tz::ren::animation_renderer& scene_renderer::get_renderer()
	{
		return this->renderer;
	}

	const tz::ren::animation_renderer& scene_renderer::get_renderer() const
	{
		return this->renderer;
	}

	tz::vec4 scene_renderer::get_clear_colour() const
	{
		const tz::gl::renderer& ren = tz::gl::get_device().get_renderer(this->renderer.get_render_pass());
		return ren.get_state().graphics.clear_colour;
	}

	void scene_renderer::set_clear_colour(tz::vec4 rgba)
	{
		tz::gl::renderer& ren = tz::gl::get_device().get_renderer(this->renderer.get_render_pass());
		ren.edit(tz::gl::RendererEditBuilder{}
		.render_state
		({
			.clear_colour = rgba
		})
		.build());
	}

	/*static*/ std::vector<tz::gl::buffer_resource> scene_renderer::evaluate_extra_buffers()
	{
		std::vector<tz::gl::buffer_resource> ret = {};
		ret.push_back(tz::gl::buffer_resource::from_one(light_data{},
		{
			.access = tz::gl::resource_access::dynamic_access
		}));	
		return ret;
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
		rinfo.set_options({tz::gl::renderer_option::no_depth_testing, tz::gl::renderer_option::no_present});
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
		return this->renderer->get_renderer().animated_object_get_subobjects(this->entry.obj).size();
	}

	tz::ren::animation_renderer::texture_locator scene_element::object_get_texture(tz::ren::animation_renderer::object_handle h, std::size_t bound_texture_id) const
	{
		return this->renderer->get_renderer().object_get_texture(h, bound_texture_id);
	}

	void scene_element::object_set_texture(tz::ren::animation_renderer::object_handle h, std::size_t bound_texture_id, tz::ren::animation_renderer::texture_locator tloc)
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

	std::size_t scene_element::get_animation_count() const
	{
		auto gltfh = this->renderer->get_renderer().animated_object_get_gltf(this->entry.obj);
		return this->renderer->get_renderer().gltf_get_animation_count(gltfh);
	}

	std::optional<std::size_t> scene_element::get_playing_animation_id() const
	{
		auto anims = this->renderer->get_renderer().animated_object_get_playing_animations(this->entry.obj);
		if(anims.size())
		{
			return anims.front().animation_id;
		}
		return std::nullopt;
	}

	std::string scene_element::get_playing_animation_name() const
	{
		auto maybe_id = this->get_playing_animation_id();
		if(!maybe_id.has_value())
		{
			return "";
		}
		tz::ren::animation_renderer::gltf_handle gltfh = this->renderer->get_renderer().animated_object_get_gltf(this->entry.obj);
		return std::string{this->renderer->get_renderer().gltf_get_animation_name(gltfh, maybe_id.value())};
	}

	std::string_view scene_element::get_animation_name(std::size_t anim_id) const
	{
		auto gltfh = this->renderer->get_renderer().animated_object_get_gltf(this->entry.obj);
		return this->renderer->get_renderer().gltf_get_animation_name(gltfh, anim_id);
	}

	void scene_element::play_animation(std::size_t anim_id, bool loop, float time_warp)
	{
		this->renderer->get_renderer().animated_object_play_animation(this->entry.obj, {.animation_id = anim_id, .loop = loop, .time_warp = time_warp});
	}

	bool scene_element::play_animation_by_name(std::string_view name, bool loop, float time_warp)
	{
		return this->renderer->get_renderer().animated_object_play_animation_by_name(this->entry.obj, name, {.loop = loop, .time_warp = time_warp});
	}

	void scene_element::queue_animation(std::size_t anim_id, bool loop, float time_warp)
	{
		this->renderer->get_renderer().animated_object_queue_animation(this->entry.obj, {.animation_id = anim_id, .loop = loop, .time_warp = time_warp});
	}

	bool scene_element::queue_animation_by_name(std::string_view name, bool loop, float time_warp)
	{
		return this->renderer->get_renderer().animated_object_queue_animation_by_name(this->entry.obj, name, {.loop = loop, .time_warp = time_warp});
	}

	void scene_element::skip_animation()
	{
		this->renderer->get_renderer().animated_object_skip_animation(this->entry.obj);
	}

	void scene_element::skip_all_animations()
	{
		this->renderer->get_renderer().animated_object_skip_all_animations(this->entry.obj);
	}

	void scene_element::halt_animation()
	{
		this->renderer->get_renderer().animated_object_skip_animation(this->entry.obj);
	}

	// Lua API

	int impl_rn_rendered_text::set_position(tz::lua::state& state)
	{
		auto [_, posx, posy] = tz::lua::parse_args<tz::lua::nil, float, float>(state);
		this->trs.translate[0] = posx;
		this->trs.translate[1] = posy;
		this->renderer->get_text_renderer().string_set_transform(this->sh, this->trs);
		return 0;
	}

	int impl_rn_scene_renderer::get_camera_position(tz::lua::state& state)
	{
		tz::vec2 ret = this->renderer->get_camera_position();
		state.stack_push_float(ret[0]);
		state.stack_push_float(ret[1]);
		return 2;
	}
	
	int impl_rn_scene_renderer::set_camera_position(tz::lua::state& state)
	{
		auto [_, camx, camy] = tz::lua::parse_args<tz::lua::nil, float, float>(state);
		game::messaging::scene_insert_message
		({
			.operation = game::messaging::scene_operation::renderer_set_camera_position,
			.value = tz::vec2{camx, camy}
		});
		return 0;
	}

	int impl_rn_scene_renderer::get_clear_colour(tz::lua::state& state)
	{
		tz::vec4 clear_colour = this->renderer->get_clear_colour();
		state.stack_push_float(clear_colour[0]);
		state.stack_push_float(clear_colour[1]);
		state.stack_push_float(clear_colour[2]);
		state.stack_push_float(clear_colour[3]);
		return 4;
	}

	int impl_rn_scene_renderer::set_clear_colour(tz::lua::state& state)
	{
		auto [_, r, g, b, a] = tz::lua::parse_args<tz::lua::nil, float, float, float, float>(state);

		game::messaging::scene_insert_message
		({
			.operation = game::messaging::scene_operation::renderer_set_clear_colour,
			.uuid = std::numeric_limits<entity_uuid>::max(),
			.value = tz::vec4{r, g, b, a}
		});
		return 0;
	}

	int impl_rn_scene_renderer::add_texture(tz::lua::state& state)
	{
		auto [_, name, relpath] = tz::lua::parse_args<tz::lua::nil, std::string, std::string>(state);
		game::messaging::scene_insert_message
		({
			.operation = game::messaging::scene_operation::renderer_add_texture,
			.uuid = std::numeric_limits<entity_uuid>::max(),
			.value = std::pair<std::string, std::string>{name, relpath}
		});
		return 0;
	}

	int impl_rn_scene_renderer::add_string(tz::lua::state& state)
	{
		TZ_PROFZONE("scene renderer - add string", 0xFFFFAAEE);
		auto [_, posx, posy, size, str, r, g, b] = tz::lua::parse_args<tz::lua::nil, float, float, float, std::string, float, float, float>(state);
		tz::trs trs{.translate = {posx, posy, 0.0f}, .scale = tz::vec3::filled(size)};
		auto stringh = this->renderer->get_text_renderer().add_string(static_cast<tz::hanval>(0), trs, str, {r, g, b});
		impl_rn_rendered_text ret{.renderer = this->renderer, .sh = stringh, .trs = trs};
		LUA_CLASS_PUSH(state, impl_rn_rendered_text, ret);
		return 1;
	}

	int impl_rn_scene_renderer::remove_string(tz::lua::state& state)
	{
		auto& text = state.stack_get_userdata<impl_rn_rendered_text>(2);
		this->renderer->get_text_renderer().remove_string(text.sh);
		return 0;
	}

	int impl_rn_scene_renderer::clear_strings(tz::lua::state& state)
	{
		this->renderer->get_text_renderer().clear_strings();
		return 0;
	}

	void scene_renderer::lua_initialise(tz::lua::state& state)
	{
		TZ_PROFZONE("scene renderer - lua initialise", 0xFFFF4488);
		state.new_type("impl_rn_rendered_text", LUA_CLASS_NAME(impl_rn_rendered_text)::registers);
		state.new_type("impl_rn_scene_renderer", LUA_CLASS_NAME(impl_rn_scene_renderer)::registers);
	}

}
