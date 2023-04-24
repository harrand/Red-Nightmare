#include "gamelib/render/scene_renderer.hpp"
#include "tz/gl/output.hpp"
#include "tz/gl/imported_shaders.hpp"

#include ImportedShaderHeader(empty, vertex)
#include ImportedShaderHeader(empty, fragment)
#include ImportedShaderHeader(rain, vertex)
#include ImportedShaderHeader(rain, fragment)

namespace rnlib
{
	struct global_effect_data
	{
		std::uint32_t time;
		float pad0;
		tz::vec2 monitor_dimensions;
		tz::vec2 window_dimensions;
	};

	scene_renderer::scene_renderer()
	{
		this->global_storage = [this]()->tz::gl::renderer_handle
		{
			tz::gl::renderer_info rinfo;
			rinfo.debug_name("Global Effect Storage Renderer");
			rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(empty, vertex));
			rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(empty, fragment));
			this->global_buffer = rinfo.add_resource(tz::gl::buffer_resource::from_one(global_effect_data{},
			{
				.access = tz::gl::resource_access::dynamic_fixed
			}));
			return tz::gl::get_device().create_renderer(rinfo);
		}();

		for(std::size_t i = 1; i < static_cast<std::size_t>(effect_type::_count); i++)
		{
			switch(static_cast<effect_type>(i))
			{
				case effect_type::rain:
					this->effects[i-1] = scene_renderer::make_rain_effect();
				break;
			}
		}
		tz::assert(std::none_of(this->effects.begin(), this->effects.end(), [](const effect_data& effect)
		{
			return effect.storage == tz::nullhand || effect.effect == tz::nullhand || effect.storage_resource == tz::nullhand;
		}), "One or more effects were not initialised properly.");
	}

	std::array<tz::gl::renderer_handle, static_cast<std::size_t>(effect_type::_count) - 1> scene_renderer::get_effects() const
	{
		std::array<tz::gl::renderer_handle, static_cast<std::size_t>(effect_type::_count) - 1> ret;
		for(std::size_t i = 0; i < this->effects.size(); i++)
		{
			ret[i] = this->effects[i].effect;
		}
		return ret;
	}

	void scene_renderer::update()
	{
		global_effect_data& gdata = tz::gl::get_device().get_renderer(this->global_storage).get_resource(this->global_buffer)->data_as<global_effect_data>().front();
		gdata.time = (tz::system_time() - this->creation).millis<std::uint32_t>();
	}

	scene_renderer::effect_data scene_renderer::make_rain_effect()
	{
		scene_renderer::effect_data ret;

		tz::gl::renderer_info sinfo;
		sinfo.debug_name("Rain Effect Storage");
		ret.storage_resource = sinfo.add_resource(tz::gl::image_resource::from_uninitialised
		({
			.format = tz::gl::image_format::BGRA32,
			.dimensions = tz::window().get_dimensions(),
			.flags = {tz::gl::resource_flag::renderer_output, tz::gl::resource_flag::image_wrap_repeat}
		}));
		sinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(empty, vertex));
		sinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(empty, fragment));
		ret.storage = tz::gl::get_device().create_renderer(sinfo);

		tz::gl::renderer_info rinfo;
		rinfo.debug_name("Rain Effect Renderer");
		rinfo.state().graphics.tri_count = 1;
		rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(rain, vertex));
		rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(rain, fragment));
		rinfo.set_options({tz::gl::renderer_option::no_depth_testing});
		rinfo.ref_resource(this->global_storage, this->global_buffer);
		// set output to the storage renderer's image.
		rinfo.set_output(tz::gl::image_output
		{{
			.colours = {tz::gl::get_device().get_renderer(ret.storage).get_component(ret.storage_resource)}
		}});
		ret.effect = tz::gl::get_device().create_renderer(rinfo);

		return ret;
	}
}
