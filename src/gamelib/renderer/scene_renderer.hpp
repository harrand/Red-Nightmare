#ifndef RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP
#define RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP
#include "tz/ren/animation.hpp"
#include "tz/io/gltf.hpp"
#include "tz/core/debug.hpp"

#include "tz/core/imported_text.hpp"
#include ImportedTextHeader(plane, glb)
#include ImportedTextHeader(human_animated_textured, glb)

namespace game::render
{
	class scene_renderer : private tz::ren::animation_renderer
	{
	public:
		scene_renderer();

		enum class model
		{
			quad,
			humanoid,
			_count
		};

		struct entry
		{
			tz::handle<tz::ren::animation_renderer::asset_package> handle;
			model m;
		};

		static tz::io::gltf get_model(model m)
		{
			switch(m)
			{
				case model::quad:
					return tz::io::gltf::from_memory(ImportedTextData(plane, glb));
				break;
				case model::humanoid:
					return tz::io::gltf::from_memory(ImportedTextData(human_animated_textured, glb));
				break;
				default:
					tz::error("Unsupported model");
					return {};
				break;
			}
		}
		entry add_model(model m);
		void update(float delta);
		void dbgui();
	private:
		void update_camera(float delta);

		int impl_mouse_scroll_delta = 0;
		object_handle root = tz::nullhand;
		tz::vec2 view_bounds = {64.0f, 64.0f};
		std::vector<tz::ren::animation_renderer::asset_package> entries = {};
		std::array<tz::ren::animation_renderer::asset_package, static_cast<int>(model::_count)> base_models = {};
	};
}

#endif // RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP