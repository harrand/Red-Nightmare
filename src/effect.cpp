#include "effect.hpp"
#include "tz/core/peripherals/monitor.hpp"
#include "tz/gl/imported_shaders.hpp"
#include "tz/gl/device.hpp"
#include "tz/gl/output.hpp"
#include "hdk/debug.hpp"
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
		hdk::vec2 monitor_dimensions;
		hdk::vec2 window_dimensions;
	};

	struct LightLayerImplData
	{
		hdk::vec2 level_dimensions = hdk::vec2::zero();
	};

	EffectManager::EffectManager()
	{
		constexpr std::size_t effect_count = static_cast<std::size_t>(EffectID::Count);
		this->global_storage = [this]()->tz::gl::RendererHandle
		{
			tz::gl::RendererInfo rinfo;
			rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(empty, vertex));
			rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(empty, fragment));
			this->global_buffer = rinfo.add_resource(tz::gl::BufferResource::from_one(GlobalEffectData{},
			{
				.access = tz::gl::ResourceAccess::DynamicFixed
			}));
			rinfo.debug_name("Global Storage Meta Renderer");
			
			return tz::gl::device().create_renderer(rinfo);
		}();
		this->effect_storage_renderers.resize(effect_count, hdk::nullhand);
		this->effect_renderers.resize(effect_count, hdk::nullhand);
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
					hdk::error("Unrecognised EffectID %zu", i);
				break;
			}
		}
	}

	EffectManager::~EffectManager()
	{

		for(tz::gl::RendererHandle rh : this->effect_storage_renderers)
		{
			if(rh != hdk::nullhand)
			{
				tz::gl::device().destroy_renderer(rh);
			}
		}
	}

	void EffectManager::notify_level_dimensions(hdk::vec2 level_dimensions)
	{
		LightLayerImplData& ld = tz::gl::device().get_renderer(this->effect_renderers[static_cast<int>(EffectID::LightLayer)]).get_resource(this->light_layer_impl_buffer)->data_as<LightLayerImplData>().front();
		ld.level_dimensions = level_dimensions;
	}

	void EffectManager::update(EffectIDs ids)
	{
		GlobalEffectData& gdata = tz::gl::device().get_renderer(this->global_storage).get_resource(this->global_buffer)->data_as<GlobalEffectData>().front();
		tz::gl::device().get_renderer(this->global_storage).render();

		gdata.time = (tz::system_time() - this->creation).millis<std::uint32_t>();
		gdata.monitor_dimensions = static_cast<hdk::vec2>(tz::get_default_monitor().screen_dimensions);
		gdata.window_dimensions = hdk::vec2(tz::window().get_width(), tz::window().get_height());

		for(EffectID id : ids)
		{
			if(id == EffectID::None)
			{
				continue;
			}
			auto h = this->effect_renderers[static_cast<std::size_t>(id)];
			tz::gl::device().get_renderer(h).render(1);
		}
	}

	tz::gl::ImageComponent* EffectManager::get_effect_component(EffectID id)
	{
		auto h = this->effect_storage_renderers[static_cast<std::size_t>(id)];
		if(h == hdk::nullhand)
		{
			hdk::assert(id == EffectID::None, "Detected nullhand RendererHandle for non-null effect id %zu", static_cast<std::size_t>(id));
			return nullptr;
		}
		tz::gl::Renderer& renderer = tz::gl::device().get_renderer(h);
		return static_cast<tz::gl::ImageComponent*>(renderer.get_component([this](EffectID id)->tz::gl::ResourceHandle
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
					hdk::error("Could not locate effect ImageComponent for EffectID %zu", static_cast<std::size_t>(id));
					return hdk::nullhand;
				break;
			}
		}(id)));
	}

	tz::gl::BufferComponent* EffectManager::get_point_light_buffer()
	{
		auto& renderer = tz::gl::device().get_renderer(this->effect_renderers[static_cast<std::size_t>(EffectID::LightLayer)]);
		return static_cast<tz::gl::BufferComponent*>(renderer.get_component(this->point_light_buffer));
	}

	std::span<const PointLight> EffectManager::point_lights() const
	{
		const auto& renderer = tz::gl::device().get_renderer(this->effect_renderers[static_cast<std::size_t>(EffectID::LightLayer)]);
		return renderer.get_resource(this->point_light_buffer)->data_as<const PointLight>();
	}

	std::span<PointLight> EffectManager::point_lights()
	{
		auto& renderer = tz::gl::device().get_renderer(this->effect_renderers[static_cast<std::size_t>(EffectID::LightLayer)]);
		return renderer.get_resource(this->point_light_buffer)->data_as<PointLight>();
	}

	void EffectManager::dbgui_lights()
	{
		auto lights = this->point_lights();
		static int light_cursor = 0;
		ImGui::SliderInt("Light ID", &light_cursor, 0, lights.size());
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

	tz::gl::RendererHandle EffectManager::make_rain_storage()
	{
		tz::gl::RendererInfo rinfo;
		rinfo.debug_name("Rain Effect Storage");
		this->rain_storage = rinfo.add_resource(tz::gl::ImageResource::from_uninitialised
		({
			.format = tz::gl::ImageFormat::BGRA32,
			.dimensions = static_cast<hdk::vec2ui>(hdk::vec2{tz::window().get_width(), tz::window().get_height()}),
			.flags = {tz::gl::ResourceFlag::RendererOutput, tz::gl::ResourceFlag::ImageWrapRepeat}
		}));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(empty, vertex));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(empty, fragment));
		return tz::gl::device().create_renderer(rinfo);
	}

	tz::gl::RendererHandle EffectManager::make_rain_effect()
	{
		tz::gl::RendererInfo rinfo;
		rinfo.debug_name("Rain Effect Renderer");
		rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(rain, vertex));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(rain, fragment));
		rinfo.set_options({tz::gl::RendererOption::RenderWait, tz::gl::RendererOption::NoDepthTesting});
		rinfo.ref_resource(this->global_storage, this->global_buffer);
		auto& storage_renderer = tz::gl::device().get_renderer(this->effect_storage_renderers[static_cast<std::size_t>(EffectID::Rain)]);
		rinfo.set_output(tz::gl::ImageOutput
		{{
			.colours = {storage_renderer.get_component(this->rain_storage)}
		}});
		return tz::gl::device().create_renderer(rinfo);
	}

	tz::gl::RendererHandle EffectManager::make_snow_storage()
	{
		tz::gl::RendererInfo rinfo;
		rinfo.debug_name("Snow Effect Storage");
		this->snow_storage = rinfo.add_resource(tz::gl::ImageResource::from_uninitialised
		({
			.format = tz::gl::ImageFormat::BGRA32,
			.dimensions = static_cast<hdk::vec2ui>(hdk::vec2{tz::window().get_width(), tz::window().get_height()}),
			.flags = {tz::gl::ResourceFlag::RendererOutput, tz::gl::ResourceFlag::ImageWrapRepeat}
		}));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(empty, vertex));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(empty, fragment));
		return tz::gl::device().create_renderer(rinfo);
	}

	tz::gl::RendererHandle EffectManager::make_snow_effect()
	{
		tz::gl::RendererInfo rinfo;
		rinfo.debug_name("Snow Effect Renderer");
		rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(snow, vertex));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(snow, fragment));
		rinfo.set_options({tz::gl::RendererOption::RenderWait, tz::gl::RendererOption::NoDepthTesting});
		rinfo.ref_resource(this->global_storage, this->global_buffer);
		auto& storage_renderer = tz::gl::device().get_renderer(this->effect_storage_renderers[static_cast<std::size_t>(EffectID::Snow)]);
		rinfo.set_output(tz::gl::ImageOutput
		{{
			.colours = {storage_renderer.get_component(this->snow_storage)}
		}});
		return tz::gl::device().create_renderer(rinfo);
	}

	tz::gl::RendererHandle EffectManager::make_light_layer_storage()
	{
		tz::gl::RendererInfo rinfo;
		rinfo.debug_name("Light Layer Storage");
		hdk::vec2ui mondims = tz::get_default_monitor().screen_dimensions;
		auto monsize = (mondims[0] + mondims[1]) / 2;
		mondims[0] = monsize;
		mondims[1] = monsize;
		this->light_layer_storage = rinfo.add_resource(tz::gl::ImageResource::from_uninitialised
		({
			.format = tz::gl::ImageFormat::BGRA32,
			.dimensions = mondims * 2,
			.flags = {tz::gl::ResourceFlag::RendererOutput, tz::gl::ResourceFlag::ImageWrapRepeat}
		}));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(empty, vertex));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(empty, fragment));
		return tz::gl::device().create_renderer(rinfo);
	}

	tz::gl::RendererHandle EffectManager::make_light_layer_effect()
	{
		tz::gl::RendererInfo rinfo;
		rinfo.debug_name("Light Layer Effect");
		rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(light, vertex));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(light, fragment));
		rinfo.set_options({tz::gl::RendererOption::RenderWait, tz::gl::RendererOption::NoDepthTesting});
		rinfo.ref_resource(this->global_storage, this->global_buffer);
		this->light_layer_impl_buffer = rinfo.add_resource(tz::gl::BufferResource::from_one(LightLayerImplData{},
		{
			.access = tz::gl::ResourceAccess::DynamicFixed
		}));
		std::array<PointLight, 64> data;
		std::fill(data.begin(), data.end(), PointLight{});
		data[0].position = hdk::vec2(15.0f, 7.0f);
		data[0].colour = hdk::vec3(1.0f, 0.0f, 0.0f);
		data[0].power = 5.0f;
		this->point_light_buffer = rinfo.add_resource(tz::gl::BufferResource::from_many(data,
		{
			.access = tz::gl::ResourceAccess::DynamicFixed
		}));
		auto& storage_renderer = tz::gl::device().get_renderer(this->effect_storage_renderers[static_cast<std::size_t>(EffectID::LightLayer)]);
		rinfo.set_output(tz::gl::ImageOutput
		{{
			.colours = {storage_renderer.get_component(this->light_layer_storage)}
		}});
		return tz::gl::device().create_renderer(rinfo);
	}

//--------------------------------------------------------------------------------------------------

	std::unique_ptr<EffectManager> emgr = nullptr;

	namespace effects_impl
	{
		void initialise()
		{
			hdk::assert(emgr == nullptr, "Initialise: Double initialise detected");
			emgr = std::make_unique<EffectManager>();
		}

		void terminate()
		{
			hdk::assert(emgr != nullptr, "Terminate: Double terminate, or no initialise detected");
			emgr = nullptr;
		}
	}

	EffectManager& effects()
	{
		hdk::assert(emgr != nullptr, "game::effects(): No initialise detected");
		return *emgr;
	}
}
