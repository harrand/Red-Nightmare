#ifndef REDNIGHTMARE_EFFECT_HPP
#define REDNIGHTMARE_EFFECT_HPP
#include "tz/core/time.hpp"
#include "tz/gl/resource.hpp"
#include "tz/gl/component.hpp"
#include "tz/gl/renderer.hpp"

namespace game
{
	enum class EffectID
	{
		None,
		Rain,
		Count
	};

	using EffectIDs = tz::EnumField<EffectID>;

	class EffectManager
	{
	public:
		EffectManager();
		~EffectManager();
		EffectManager(const EffectManager& copy) = delete;
		EffectManager(EffectManager&& move) = delete;

		void update(EffectIDs ids = {});
		tz::gl::ImageComponent* get_effect_component(EffectID id);
	private:
		tz::gl::RendererHandle make_rain_storage();
		tz::gl::RendererHandle make_rain_effect();
		tz::gl::RendererHandle global_storage = hdk::nullhand;
		tz::gl::ResourceHandle global_buffer = hdk::nullhand;
		std::vector<tz::gl::RendererHandle> effect_storage_renderers = {};
		std::vector<tz::gl::RendererHandle> effect_renderers = {};
		tz::gl::ResourceHandle rain_storage = hdk::nullhand;
		tz::Duration creation = tz::system_time();
	};

	namespace effects_impl
	{
		void initialise();
		void terminate();
	}

	EffectManager& effects();
}

#endif // REDNIGHTMARE_EFFECT_HPP
