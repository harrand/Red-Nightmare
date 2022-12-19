#include "effect.hpp"
#include "tz/gl/imported_shaders.hpp"
#include "tz/gl/device.hpp"
#include "tz/gl/output.hpp"
#include "hdk/debug.hpp"
#include <memory>

#include ImportedShaderHeader(empty, vertex)
#include ImportedShaderHeader(empty, fragment)
#include ImportedShaderHeader(rain, vertex)
#include ImportedShaderHeader(rain, fragment)

namespace game
{
	EffectManager::EffectManager()
	{
		constexpr std::size_t effect_count = static_cast<std::size_t>(EffectID::Count);
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

	void EffectManager::update(EffectIDs ids)
	{
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
				default:
					hdk::error("Could not locate effect ImageComponent for EffectID %zu", static_cast<std::size_t>(id));
					return hdk::nullhand;
				break;
			}
		}(id)));
	}

	tz::gl::RendererHandle EffectManager::make_rain_storage()
	{
		tz::gl::RendererInfo rinfo;
		rinfo.debug_name("Rain Effect Storage");
		this->rain_storage = rinfo.add_resource(tz::gl::ImageResource::from_uninitialised
		({
			.format = tz::gl::ImageFormat::BGRA32,
			.dimensions = static_cast<hdk::vec2ui>(hdk::vec2{tz::window().get_width(), tz::window().get_height()}),
			.flags = {tz::gl::ResourceFlag::RendererOutput}
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
		auto& storage_renderer = tz::gl::device().get_renderer(this->effect_storage_renderers[static_cast<std::size_t>(EffectID::Rain)]);
		rinfo.set_output(tz::gl::ImageOutput
		{{
			.colours = {storage_renderer.get_component(this->rain_storage)}
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
