#ifndef REDNIGHTMARE_EFFECT_HPP
#define REDNIGHTMARE_EFFECT_HPP
#include "light.hpp"
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
		Snow,
		LightLayer,
		Count
	};

	using EffectIDs = tz::enum_field<EffectID>;

	class EffectManager
	{
	public:
		EffectManager();
		~EffectManager();
		EffectManager(const EffectManager& copy) = delete;
		EffectManager(EffectManager&& move) = delete;

		void notify_level_dimensions(tz::vec2 level_dimensions);
		void update(EffectIDs ids = {});
		tz::gl::image_component* get_effect_component(EffectID id);
		tz::gl::buffer_component* get_point_light_buffer();
		std::span<const PointLight> point_lights() const;
		std::span<PointLight> point_lights();

		void dbgui_lights();
	private:
		tz::gl::renderer_handle make_rain_storage();
		tz::gl::renderer_handle make_rain_effect();
		tz::gl::renderer_handle make_snow_storage();
		tz::gl::renderer_handle make_snow_effect();
		tz::gl::renderer_handle make_light_layer_storage();
		tz::gl::renderer_handle make_light_layer_effect();
		tz::gl::renderer_handle global_storage = tz::nullhand;
		tz::gl::resource_handle global_buffer = tz::nullhand;
		std::vector<tz::gl::renderer_handle> effect_storage_renderers = {};
		std::vector<tz::gl::renderer_handle> effect_renderers = {};
		tz::gl::resource_handle rain_storage = tz::nullhand;
		tz::gl::resource_handle snow_storage = tz::nullhand;
		tz::gl::resource_handle light_layer_storage = tz::nullhand;
		tz::gl::resource_handle point_light_buffer = tz::nullhand;
		tz::gl::resource_handle light_layer_impl_buffer = tz::nullhand;
		tz::duration creation = tz::system_time();
	};

	namespace effects_impl
	{
		void initialise();
		void terminate();
	}

	EffectManager& effects();
}

#endif // REDNIGHTMARE_EFFECT_HPP
