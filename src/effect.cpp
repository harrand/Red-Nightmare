#include "effect.hpp"
#include "effects/light.tzsl"
#include "tz/wsi/monitor.hpp"
#include "tz/gl/imported_shaders.hpp"
#include "tz/gl/device.hpp"
#include "tz/gl/output.hpp"
#include "tz/core/debug.hpp"
#include <memory>

#include ImportedShaderHeader(empty, vertex)
#include ImportedShaderHeader(empty, fragment)
#include ImportedShaderHeader(rain, vertex)
#include ImportedShaderHeader(rain, fragment)
#include ImportedShaderHeader(snow, vertex)
#include ImportedShaderHeader(snow, fragment)
#include ImportedShaderHeader(light, vertex)
#include ImportedShaderHeader(light, fragment)

namespace game
{
	struct GlobalEffectData
	{
		std::uint32_t time;
		float pad0[1];
		tz::vec2 monitor_dimensions;
		tz::vec2 window_dimensions;
	};

	struct LightLayerImplData
	{
		tz::vec2 level_dimensions = tz::vec2::zero();
	};

	EffectManager::EffectManager()
	{
		constexpr std::size_t effect_count = static_cast<std::size_t>(EffectID::Count);
		this->global_storage = [this]()->tz::gl::renderer_handle
		{
			tz::gl::renderer_info rinfo;
			rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(empty, vertex));
			rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(empty, fragment));
			this->global_buffer = rinfo.add_resource(tz::gl::buffer_resource::from_one(GlobalEffectData{},
			{
				.access = tz::gl::resource_access::dynamic_fixed
			}));
			rinfo.debug_name("Global Storage Meta Renderer");
			
			return tz::gl::get_device().create_renderer(rinfo);
		}();
		this->effect_storage_renderers.resize(effect_count, tz::nullhand);
		this->effect_renderers.resize(effect_count, tz::nullhand);
		for(std::size_t i = 0; i < effect_count; i++)
		{
			switch(static_cast<EffectID>(i))
			{
				case EffectID::None: break;
				case EffectID::Rain:
					this->effect_storage_renderers[i] = this->make_rain_storage();
					this->effect_renderers[i] = this->make_rain_effect();
				break;
				case EffectID::Snow:
					this->effect_storage_renderers[i] = this->make_snow_storage();
					this->effect_renderers[i] = this->make_snow_effect();
				break;
				case EffectID::LightLayer:
					this->effect_storage_renderers[i] = this->make_light_layer_storage();
					this->effect_renderers[i] = this->make_light_layer_effect();
				break;
				default:
					tz::error("Unrecognised EffectID %zu", i);
				break;
			}
		}
	}

	EffectManager::~EffectManager()
	{

		for(tz::gl::renderer_handle rh : this->effect_storage_renderers)
		{
			if(rh != tz::nullhand)
			{
				tz::gl::get_device().destroy_renderer(rh);
			}
		}
	}

	void EffectManager::notify_level_dimensions(tz::vec2 level_dimensions)
	{
		LightLayerImplData& ld = tz::gl::get_device().get_renderer(this->effect_renderers[static_cast<int>(EffectID::LightLayer)]).get_resource(this->light_layer_impl_buffer)->data_as<LightLayerImplData>().front();
		ld.level_dimensions = level_dimensions;
	}

	void EffectManager::update(EffectIDs ids)
	{
		GlobalEffectData& gdata = tz::gl::get_device().get_renderer(this->global_storage).get_resource(this->global_buffer)->data_as<GlobalEffectData>().front();

		gdata.time = (tz::system_time() - this->creation).millis<std::uint32_t>();
		gdata.monitor_dimensions = static_cast<tz::vec2>(tz::wsi::get_monitors().front().dimensions);
		gdata.window_dimensions = static_cast<tz::vec2>(tz::window().get_dimensions());

		for(EffectID id : ids)
		{
			if(id == EffectID::None)
			{
				continue;
			}
			auto h = this->effect_renderers[static_cast<std::size_t>(id)];
			tz::gl::get_device().get_renderer(h).render(1);
		}
	}

	tz::gl::image_component* EffectManager::get_effect_component(EffectID id)
	{
		auto h = this->effect_storage_renderers[static_cast<std::size_t>(id)];
		if(h == tz::nullhand)
		{
			tz::assert(id == EffectID::None, "Detected nullhand renderer_handle for non-null effect id %zu", static_cast<std::size_t>(id));
			return nullptr;
		}
		tz::gl::renderer& renderer = tz::gl::get_device().get_renderer(h);
		return static_cast<tz::gl::image_component*>(renderer.get_component([this](EffectID id)->tz::gl::resource_handle
		{
			switch(id)
			{
				case EffectID::Rain:
					return this->rain_storage;
				break;
				case EffectID::Snow:
					return this->snow_storage;
				break;
				case EffectID::LightLayer:
					return this->light_layer_storage;
				break;
				default:
					tz::error("Could not locate effect image_component for EffectID %zu", static_cast<std::size_t>(id));
					return tz::nullhand;
				break;
			}
		}(id)));
	}

	tz::gl::buffer_component* EffectManager::get_point_light_buffer()
	{
		auto& renderer = tz::gl::get_device().get_renderer(this->effect_renderers[static_cast<std::size_t>(EffectID::LightLayer)]);
		return static_cast<tz::gl::buffer_component*>(renderer.get_component(this->point_light_buffer));
	}

	std::span<const PointLight> EffectManager::point_lights() const
	{
		const auto& renderer = tz::gl::get_device().get_renderer(this->effect_renderers[static_cast<std::size_t>(EffectID::LightLayer)]);
		return renderer.get_resource(this->point_light_buffer)->data_as<const PointLight>();
	}

	std::span<PointLight> EffectManager::point_lights()
	{
		auto& renderer = tz::gl::get_device().get_renderer(this->effect_renderers[static_cast<std::size_t>(EffectID::LightLayer)]);
		return renderer.get_resource(this->point_light_buffer)->data_as<PointLight>();
	}

	void EffectManager::dbgui_lights()
	{
		auto lights = this->point_lights();
		static int light_cursor = 0;
		ImGui::SliderInt("Light ID", &light_cursor, 0, lights.size() - 1);
		ImGui::Spacing();
		{
			ImGui::Indent();
			auto& l = lights[light_cursor];
			ImGui::DragFloat2("Position", l.position.data().data(), 0.01f, 0.0f, 128.0f);
			ImGui::ColorEdit3("Colour", l.colour.data().data());
			ImGui::SliderFloat("Power", &l.power, 0.0f, 64.0f);
			ImGui::Unindent();
		}
	}

	tz::gl::renderer_handle EffectManager::make_rain_storage()
	{
		tz::gl::renderer_info rinfo;
		rinfo.debug_name("Rain Effect Storage");
		this->rain_storage = rinfo.add_resource(tz::gl::image_resource::from_uninitialised
		({
			.format = tz::gl::image_format::BGRA32,
			.dimensions = tz::window().get_dimensions(),
			.flags = {tz::gl::resource_flag::renderer_output, tz::gl::resource_flag::image_wrap_repeat}
		}));
		rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(empty, vertex));
		rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(empty, fragment));
		return tz::gl::get_device().create_renderer(rinfo);
	}

	tz::gl::renderer_handle EffectManager::make_rain_effect()
	{
		tz::gl::renderer_info rinfo;
		rinfo.debug_name("Rain Effect Renderer");
		rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(rain, vertex));
		rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(rain, fragment));
		rinfo.set_options({tz::gl::renderer_option::render_wait, tz::gl::renderer_option::no_depth_testing});
		rinfo.ref_resource(this->global_storage, this->global_buffer);
		auto& storage_renderer = tz::gl::get_device().get_renderer(this->effect_storage_renderers[static_cast<std::size_t>(EffectID::Rain)]);
		rinfo.set_output(tz::gl::image_output
		{{
			.colours = {storage_renderer.get_component(this->rain_storage)}
		}});
		return tz::gl::get_device().create_renderer(rinfo);
	}

	tz::gl::renderer_handle EffectManager::make_snow_storage()
	{
		tz::gl::renderer_info rinfo;
		rinfo.debug_name("Snow Effect Storage");
		this->snow_storage = rinfo.add_resource(tz::gl::image_resource::from_uninitialised
		({
			.format = tz::gl::image_format::BGRA32,
			.dimensions = tz::window().get_dimensions(),
			.flags = {tz::gl::resource_flag::renderer_output, tz::gl::resource_flag::image_wrap_repeat}
		}));
		rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(empty, vertex));
		rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(empty, fragment));
		return tz::gl::get_device().create_renderer(rinfo);
	}

	tz::gl::renderer_handle EffectManager::make_snow_effect()
	{
		tz::gl::renderer_info rinfo;
		rinfo.debug_name("Snow Effect Renderer");
		rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(snow, vertex));
		rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(snow, fragment));
		rinfo.set_options({tz::gl::renderer_option::render_wait, tz::gl::renderer_option::no_depth_testing});
		rinfo.ref_resource(this->global_storage, this->global_buffer);
		auto& storage_renderer = tz::gl::get_device().get_renderer(this->effect_storage_renderers[static_cast<std::size_t>(EffectID::Snow)]);
		rinfo.set_output(tz::gl::image_output
		{{
			.colours = {storage_renderer.get_component(this->snow_storage)}
		}});
		return tz::gl::get_device().create_renderer(rinfo);
	}

	tz::gl::renderer_handle EffectManager::make_light_layer_storage()
	{
		tz::gl::renderer_info rinfo;
		rinfo.debug_name("Light Layer Storage");
		tz::vec2ui mondims = tz::wsi::get_monitors().front().dimensions;
		auto monsize = (mondims[0] + mondims[1]) / 2;
		mondims[0] = monsize;
		mondims[1] = monsize;
		this->light_layer_storage = rinfo.add_resource(tz::gl::image_resource::from_uninitialised
		({
			.format = tz::gl::image_format::BGRA32,
			.dimensions = mondims * 2,
			.flags = {tz::gl::resource_flag::renderer_output, tz::gl::resource_flag::image_wrap_repeat}
		}));
		rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(empty, vertex));
		rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(empty, fragment));
		return tz::gl::get_device().create_renderer(rinfo);
	}

	tz::gl::renderer_handle EffectManager::make_light_layer_effect()
	{
		tz::gl::renderer_info rinfo;
		rinfo.debug_name("Light Layer Effect");
		rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(light, vertex));
		rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(light, fragment));
		rinfo.set_options({tz::gl::renderer_option::render_wait, tz::gl::renderer_option::no_depth_testing});
		rinfo.ref_resource(this->global_storage, this->global_buffer);
		this->light_layer_impl_buffer = rinfo.add_resource(tz::gl::buffer_resource::from_one(LightLayerImplData{},
		{
			.access = tz::gl::resource_access::dynamic_fixed
		}));
		std::array<PointLight, max_light_count> data;
		std::fill(data.begin(), data.end(), PointLight{});
		this->point_light_buffer = rinfo.add_resource(tz::gl::buffer_resource::from_many(data,
		{
			.access = tz::gl::resource_access::dynamic_fixed
		}));
		auto& storage_renderer = tz::gl::get_device().get_renderer(this->effect_storage_renderers[static_cast<std::size_t>(EffectID::LightLayer)]);
		rinfo.set_output(tz::gl::image_output
		{{
			.colours = {storage_renderer.get_component(this->light_layer_storage)}
		}});
		return tz::gl::get_device().create_renderer(rinfo);
	}

//--------------------------------------------------------------------------------------------------

	std::unique_ptr<EffectManager> emgr = nullptr;

	namespace effects_impl
	{
		void initialise()
		{
			tz::assert(emgr == nullptr, "Initialise: Double initialise detected");
			emgr = std::make_unique<EffectManager>();
		}

		void terminate()
		{
			tz::assert(emgr != nullptr, "Terminate: Double terminate, or no initialise detected");
			emgr = nullptr;
		}
	}

	EffectManager& effects()
	{
		tz::assert(emgr != nullptr, "game::effects(): No initialise detected");
		return *emgr;
	}
}
