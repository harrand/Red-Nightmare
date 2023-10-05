#ifndef RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP
#define RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP
#include "tz/ren/animation.hpp"
#include "tz/io/gltf.hpp"
#include "tz/core/debug.hpp"
#include "tz/lua/api.hpp"

#include "tz/core/imported_text.hpp"
#include ImportedTextHeader(plane, glb)
#include ImportedTextHeader(human_animated_textured, glb)

namespace game::render
{
	struct scene_element;
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

		static constexpr const char* get_model_name(model m)
		{
			return std::array<const char*, static_cast<int>(model::_count)>
			{
				"quad",
				"humanoid"
			}[static_cast<int>(m)];
		}

		struct entry
		{
			tz::ren::animation_renderer::asset_package pkg;
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
		scene_element get_element(entry e);
		void update(float delta);
		void dbgui();

		tz::ren::animation_renderer& get_renderer();
		void lua_initialise(tz::lua::state& state);
	private:
		void update_camera(float delta);

		int impl_mouse_scroll_delta = 0;
		object_handle root = tz::nullhand;
		tz::vec2 view_bounds = {64.0f, 64.0f};
		std::vector<tz::ren::animation_renderer::asset_package> entries = {};
		std::array<tz::ren::animation_renderer::asset_package, static_cast<int>(model::_count)> base_models = {};
	};

	struct scene_element
	{
		scene_renderer* renderer = nullptr;
		scene_renderer::entry entry = {};

		scene_renderer::model get_model() const;
		std::size_t get_animation_count() const;
		std::optional<std::size_t> get_playing_animation_id() const;
		std::string_view get_animation_name(std::size_t anim_id) const;
		void play_animation(std::size_t anim_id, bool loop = false);
		void queue_animation(std::size_t anim_id, bool loop = false);
	};
}

#endif // RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP