#ifndef RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP
#define RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP
#include "tz/ren/animation.hpp"
#include "tz/ren/text.hpp"
#include "tz/io/gltf.hpp"
#include "tz/gl/output.hpp"
#include "tz/core/debug.hpp"
#include "tz/lua/api.hpp"

namespace game::render
{
	struct scene_element;
	class scene_renderer
	{
	public:
		scene_renderer();
		using string_handle = tz::ren::text_renderer::string_handle;

		struct point_light_data
		{
			tz::vec3 position = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), 1.0f};
			float pad0[1] = {};
			tz::vec3 colour = {1.0f, 1.0f, 1.0f};
			float power = 0.0f;
		};
		struct light_data
		{
			tz::vec3 ambient_light_colour{1.0f, 1.0f, 1.0f};
			std::uint32_t point_light_count = 32u;
			std::array<point_light_data, 32u> point_lights = {};
		};

		struct entry
		{
			tz::ren::animation_renderer::animated_objects_handle obj;
			std::string model_name;
		};

		void add_model(std::string model_name, tz::io::gltf model);
		void remove_model(std::string model_name);

		entry add_entry(std::string model_name);
		void remove_entry(entry e);
		void clear_entries();
		scene_element get_element(entry e);
		entry entry_at(std::size_t idx) const;
		std::size_t entry_count() const;

		const tz::ren::text_renderer& get_text_renderer() const;
		tz::ren::text_renderer& get_text_renderer();

		const tz::vec2& get_view_bounds() const;
		tz::vec2 get_camera_position() const;
		void set_camera_position(tz::vec2 cam_pos);
		void update(float delta);
		void block();
		void dbgui();

		tz::vec3 get_ambient_light() const;
		void set_ambient_light(tz::vec3 rgb_light);
		std::span<const point_light_data> get_point_lights() const;
		std::span<point_light_data> get_point_lights();
		void set_point_light_capacity(unsigned int num_point_lights);

		tz::ren::animation_renderer& get_renderer();
		const tz::ren::animation_renderer& get_renderer() const;

		tz::vec4 get_clear_colour() const;
		void set_clear_colour(tz::vec4 rgba);

		void lua_initialise(tz::lua::state& state);
	private:
		static std::vector<tz::gl::buffer_resource> evaluate_extra_buffers();
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
		tz::ren::animation_renderer renderer;
		tz::ren::text_renderer text_renderer;
		int impl_mouse_scroll_delta = 0;
		tz::ren::animation_renderer::object_handle root = tz::nullhand;
		tz::vec2 view_bounds = {64.0f, 64.0f};
		std::vector<entry> entries = {};
		std::unordered_map<std::string, tz::ren::animation_renderer::gltf_handle> registered_models = {};
	};

	struct scene_element
	{
		scene_renderer* renderer = nullptr;
		scene_renderer::entry entry = {};

		std::size_t get_object_count() const;
		tz::ren::animation_renderer::texture_locator object_get_texture(tz::ren::animation_renderer::object_handle oh, std::size_t bound_texture_id) const;
		void object_set_texture(tz::ren::animation_renderer::object_handle h, std::size_t bound_texture_id, tz::ren::animation_renderer::texture_locator tloc);
		bool object_get_visibility(tz::ren::animation_renderer::object_handle h) const;
		void object_set_visibility(tz::ren::animation_renderer::object_handle, bool visibility);
		std::size_t get_animation_count() const;
		std::optional<std::size_t> get_playing_animation_id() const;
		std::string get_playing_animation_name() const;
		std::string_view get_animation_name(std::size_t anim_id) const;
		void play_animation(std::size_t anim_id, bool loop = false, float time_warp = 1.0f);
		bool play_animation_by_name(std::string_view name, bool loop = false, float time_warp = 1.0f);
		void queue_animation(std::size_t anim_id, bool loop = false, float time_warp = 1.0f);
		bool queue_animation_by_name(std::string_view name, bool loop = false, float time_warp = 1.0f);
		void skip_animation();
		void skip_all_animations();
		void halt_animation();
	};

	// LUA API

	struct impl_rn_rendered_text
	{
		scene_renderer* renderer = nullptr;
		scene_renderer::string_handle sh;
		tz::trs trs;

		int set_position(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(impl_rn_rendered_text)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(impl_rn_rendered_text, set_position)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END

	struct impl_rn_scene_renderer
	{
		scene_renderer* renderer = nullptr;
		int get_camera_position(tz::lua::state& state);
		int set_camera_position(tz::lua::state& state);

		int get_clear_colour(tz::lua::state& state);
		int set_clear_colour(tz::lua::state& state);

		int add_string(tz::lua::state& state);
		int remove_string(tz::lua::state& state);
		int clear_strings(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(impl_rn_scene_renderer)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(impl_rn_scene_renderer, get_camera_position)
			LUA_METHOD(impl_rn_scene_renderer, set_camera_position)

			LUA_METHOD(impl_rn_scene_renderer, get_clear_colour)
			LUA_METHOD(impl_rn_scene_renderer, set_clear_colour)

			LUA_METHOD(impl_rn_scene_renderer, add_string)
			LUA_METHOD(impl_rn_scene_renderer, remove_string)
			LUA_METHOD(impl_rn_scene_renderer, clear_strings)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END
}

#endif // RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP
