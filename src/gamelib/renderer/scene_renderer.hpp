#ifndef RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP
#define RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP
#include "tz/ren/animation2.hpp"
#include "tz/io/gltf.hpp"
#include "tz/gl/output.hpp"
#include "tz/core/debug.hpp"
#include "tz/lua/api.hpp"

#include "tz/core/imported_text.hpp"
#include ImportedTextHeader(plane, glb)
#include ImportedTextHeader(human_animated_textured, glb)

namespace game::render
{
	struct scene_element;
	class scene_renderer
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
			tz::ren::animation_renderer2::animated_objects_handle obj;
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
		void remove_model(entry e);
		scene_element get_element(entry e);
		entry entry_at(std::size_t idx) const;
		std::size_t entry_count() const;
		const tz::vec2& get_view_bounds() const;
		tz::vec2 get_camera_position() const;
		void update(float delta);
		void block();
		void dbgui();

		tz::ren::animation_renderer2& get_renderer();
		void lua_initialise(tz::lua::state& state);
	private:
		void update_camera(float delta);

		struct pixelate_pass_t
		{
			pixelate_pass_t();
			tz::gl::icomponent* get_background_image();
			tz::gl::icomponent* get_foreground_image();
			float& zoom_amount();
			void handle_resize(tz::gl::renderer_handle animation_render_pass);

			tz::gl::renderer_handle handle = tz::nullhand;
			tz::gl::resource_handle dimension_buffer = tz::nullhand;
			tz::gl::resource_handle zoom_buffer = tz::nullhand;
			tz::gl::resource_handle bg_image = tz::nullhand;
			tz::gl::resource_handle fg_image = tz::nullhand;
			tz::vec2ui dims_cache;
		};

		pixelate_pass_t pixelate_pass;
		tz::gl::image_output output;
		tz::ren::animation_renderer2 renderer;
		int impl_mouse_scroll_delta = 0;
		tz::ren::animation_renderer2::object_handle root = tz::nullhand;
		tz::vec2 view_bounds = {64.0f, 64.0f};
		std::vector<entry> entries = {};
	};

	struct scene_element
	{
		scene_renderer* renderer = nullptr;
		scene_renderer::entry entry = {};

		std::size_t get_object_count() const;
		tz::ren::animation_renderer2::texture_locator object_get_texture(tz::ren::animation_renderer2::object_handle oh, std::size_t bound_texture_id) const;
		void object_set_texture(tz::ren::animation_renderer2::object_handle h, std::size_t bound_texture_id, tz::ren::animation_renderer2::texture_locator tloc);
		bool object_get_visibility(tz::ren::animation_renderer2::object_handle h) const;
		void object_set_visibility(tz::ren::animation_renderer2::object_handle, bool visibility);
		scene_renderer::model get_model() const;
		std::size_t get_animation_count() const;
		std::optional<std::size_t> get_playing_animation_id() const;
		std::string_view get_animation_name(std::size_t anim_id) const;
		void play_animation(std::size_t anim_id, bool loop = false);
		void queue_animation(std::size_t anim_id, bool loop = false);
		void skip_animation();
		void skip_all_animations();
		void halt_animation();
	};

	// LUA API

	struct impl_rn_scene_texture_locator
	{
		tz::ren::animation_renderer2::texture_locator tloc;
		int get_colour_tint(tz::lua::state& state);
		int set_colour_tint(tz::lua::state& state);
		int get_texture_handle(tz::lua::state& state);
		int set_texture_handle(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(impl_rn_scene_texture_locator)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(impl_rn_scene_texture_locator, get_colour_tint)
			LUA_METHOD(impl_rn_scene_texture_locator, set_colour_tint)
			LUA_METHOD(impl_rn_scene_texture_locator, get_texture_handle)
			LUA_METHOD(impl_rn_scene_texture_locator, set_texture_handle)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END

	struct impl_rn_scene_element
	{
		scene_element elem;
		int get_object_count(tz::lua::state& state);
		int object_get_texture(tz::lua::state& state);
		// note: setting texture tint affects the texture locator itself, meaning everything using the texture handle is affected.
		// if you want to just affect this specific object, use object_set_colour_tint
		int object_set_texture_tint(tz::lua::state& state);
		int object_set_texture_handle(tz::lua::state& state);
		// get/set colour tint for this specific object only. does not affect any other object, even if they use the same texture id.
		int object_get_colour_tint(tz::lua::state& state);
		int object_set_colour_tint(tz::lua::state& state);
		int object_get_visibility(tz::lua::state& state);
		int object_set_visibility(tz::lua::state& state);
		int face_forward(tz::lua::state& state);
		int face_backward(tz::lua::state& state);
		int face_left(tz::lua::state& state);
		int face_right(tz::lua::state& state);
		int rotate(tz::lua::state& state);
		int get_position(tz::lua::state& state);
		int set_position(tz::lua::state& state);
		int get_subobject_position(tz::lua::state& state);
		int get_scale(tz::lua::state& state);
		int set_scale(tz::lua::state& state);
		int get_uniform_scale(tz::lua::state& state);
		int set_uniform_scale(tz::lua::state& state);
		int get_model(tz::lua::state& state);
		int get_animation_count(tz::lua::state& state);
		int get_playing_animation_id(tz::lua::state& state);
		int is_animation_playing(tz::lua::state& state);
		int get_animation_name(tz::lua::state& state);
		int get_animation_speed(tz::lua::state& state);
		int set_animation_speed(tz::lua::state& state);
		int play_animation(tz::lua::state& state);
		int queue_animation(tz::lua::state& state);
		int skip_animation(tz::lua::state& state);
		int skip_all_animations(tz::lua::state& state);
		int halt_animation(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(impl_rn_scene_element)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(impl_rn_scene_element, get_object_count)
			LUA_METHOD(impl_rn_scene_element, object_get_texture)
			LUA_METHOD(impl_rn_scene_element, object_set_texture_tint)
			LUA_METHOD(impl_rn_scene_element, object_set_texture_handle)
			LUA_METHOD(impl_rn_scene_element, object_get_colour_tint)
			LUA_METHOD(impl_rn_scene_element, object_set_colour_tint)
			LUA_METHOD(impl_rn_scene_element, object_get_visibility)
			LUA_METHOD(impl_rn_scene_element, object_set_visibility)
			LUA_METHOD(impl_rn_scene_element, face_forward)
			LUA_METHOD(impl_rn_scene_element, face_backward)
			LUA_METHOD(impl_rn_scene_element, face_left)
			LUA_METHOD(impl_rn_scene_element, face_right)
			LUA_METHOD(impl_rn_scene_element, rotate)
			LUA_METHOD(impl_rn_scene_element, get_position)
			LUA_METHOD(impl_rn_scene_element, set_position)
			LUA_METHOD(impl_rn_scene_element, get_subobject_position)
			LUA_METHOD(impl_rn_scene_element, get_scale)
			LUA_METHOD(impl_rn_scene_element, set_scale)
			LUA_METHOD(impl_rn_scene_element, get_uniform_scale)
			LUA_METHOD(impl_rn_scene_element, set_uniform_scale)
			LUA_METHOD(impl_rn_scene_element, get_model)
			LUA_METHOD(impl_rn_scene_element, get_animation_count)
			LUA_METHOD(impl_rn_scene_element, get_playing_animation_id)
			LUA_METHOD(impl_rn_scene_element, is_animation_playing)
			LUA_METHOD(impl_rn_scene_element, get_animation_name)
			LUA_METHOD(impl_rn_scene_element, get_animation_speed)
			LUA_METHOD(impl_rn_scene_element, set_animation_speed)
			LUA_METHOD(impl_rn_scene_element, play_animation)
			LUA_METHOD(impl_rn_scene_element, queue_animation)
			LUA_METHOD(impl_rn_scene_element, skip_animation)
			LUA_METHOD(impl_rn_scene_element, skip_all_animations)
			LUA_METHOD(impl_rn_scene_element, halt_animation)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END

	struct impl_rn_scene_renderer
	{
		scene_renderer* renderer = nullptr;
		int add_model(tz::lua::state& state);
		int get_element(tz::lua::state& state);
		int element_count(tz::lua::state& state);
		int load_texture_from_disk(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(impl_rn_scene_renderer)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(impl_rn_scene_renderer, add_model)
			LUA_METHOD(impl_rn_scene_renderer, get_element)
			LUA_METHOD(impl_rn_scene_renderer, element_count)
			LUA_METHOD(impl_rn_scene_renderer, load_texture_from_disk)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END
}

#endif // RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP