#include "effect.hpp"
#include "tz/gl/imported_shaders.hpp"
#include "tz/gl/device.hpp"
#include "hdk/debug.hpp"
#include <memory>

#include ImportedShaderHeader(rain, vertex)
#include ImportedShaderHeader(rain, fragment)

namespace game
{
	EffectManager::EffectManager()
	{
		constexpr std::size_t effect_count = static_cast<std::size_t>(EffectID::Count);
		this->effect_renderers.resize(effect_count, hdk::nullhand);
		for(std::size_t i = 0; i < effect_count; i++)
		{
			switch(static_cast<EffectID>(i))
			{
				case EffectID::None: break;
				case EffectID::Rain:
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
		for(tz::gl::RendererHandle rh : this->effect_renderers)
		{
			if(rh != hdk::nullhand)
			{
				tz::gl::device().destroy_renderer(rh);
			}
		}
	}

	void EffectManager::update(EffectIDs ids)
	{

	}

	tz::gl::RendererHandle EffectManager::make_rain_effect()
	{
		tz::gl::RendererInfo rinfo;
		rinfo.debug_name("Rain Effect Renderer");
		this->rain_image = rinfo.add_resource(tz::gl::ImageResource::from_uninitialised
		({
			.format = tz::gl::ImageFormat::BGRA32,
			.dimensions = {1024u, 1024u},
			.access = tz::gl::ResourceAccess::StaticFixed,
			.flags = {tz::gl::ResourceFlag::RendererOutput},
		}));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(rain, vertex));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(rain, fragment));
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
