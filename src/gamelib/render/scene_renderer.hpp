#ifndef RNLIB_RENDER_SCENE_RENDERER_HPP
#define RNLIB_RENDER_SCENE_RENDERER_HPP
#include "tz/gl/renderer.hpp"
#include "tz/gl/resource.hpp"
#include "tz/gl/device.hpp"
#include "tz/core/time.hpp"

namespace rnlib
{
	enum class effect_type
	{
		none,
		rain,
		_count
	};
	// scenes renderers draw the background of a particular scene.
	// includes:
	// backdrop
	//	- background and foreground 
	//	(example: background is a grassy field, and foreground is a set of roads, buildings etc...)
	// a list of effects that draws over the backdrop. e.g rain and other environmental effects
	class scene_renderer
	{
	public:
		scene_renderer();
		std::array<tz::gl::renderer_handle, static_cast<std::size_t>(effect_type::_count) - 1> get_effects() const;
		void update();
		void dbgui();
		std::span<std::uint32_t> effect();
		tz::gl::renderer_handle get_layer_renderer() const;
	private:
		struct effect_data
		{
			// handle for the storage renderer.
			tz::gl::renderer_handle storage = tz::nullhand;
			// handle for the actual effect renderer.
			tz::gl::renderer_handle effect = tz::nullhand;
			// resource handle for the storage renderer's resource.
			tz::gl::resource_handle storage_resource = tz::nullhand;
			// resource handle for effect-specific data. can be anything the effect wants.
			tz::gl::resource_handle extra_resource = tz::nullhand;
		};

		effect_data make_rain_effect();
		void dbgui_rain();

		tz::gl::renderer_handle global_storage = tz::nullhand;
		tz::gl::resource_handle global_buffer = tz::nullhand;
		tz::gl::renderer_handle layer_renderer = tz::nullhand;
		tz::gl::resource_handle layer_texture_buffer = tz::nullhand;
		std::array<effect_data, static_cast<int>(effect_type::_count) - 1> effects = {};
		tz::duration creation = tz::system_time();
		float effect_timer_multiplier = 1.0f;
	};
}

#endif // RNLIB_RENDER_SCENE_RENDERER_HPP
