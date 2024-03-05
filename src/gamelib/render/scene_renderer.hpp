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
			float directional_light_power = 0.2f;
			tz::vec3 directional_light_colour = tz::vec3::filled(1.0f);
			float pad0;
			tz::vec3 directional_light_direction = {-1.0f, -0.3f, 0.0f};
			std::uint32_t point_light_count = 32u;
			std::array<point_light_data, 32u> point_lights = {};
		};

		struct entry
		{
			tz::ren::animation_renderer::animated_objects_handle obj;
			std::string model_name;

			bool operator==(const entry&) const = default;
		};

		void add_model(std::string model_name, tz::io::gltf model);
		void remove_model(std::string model_name);

		void add_texture(std::string texture_name, tz::io::image image);
		void remove_texture(std::string texture_name);

		tz::ren::animation_renderer::texture_handle get_texture(std::string texture_name) const;
		std::string get_texture_name(tz::ren::animation_renderer::texture_handle texh) const;

		entry add_entry(std::string model_name);
		void remove_entry(entry e);
		void clear_entries();
		scene_element get_element(entry e);
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

		void add_light(std::size_t light_uid, point_light_data data);
		void remove_light(std::size_t light_uid);
		point_light_data* get_light(std::size_t light_uid);
		void clear_lights();
		std::vector<std::size_t> get_all_light_uids() const;

		tz::vec3 directional_light_get_direction() const;
		void directional_light_set_direction(tz::vec3 direction);
		float directional_light_get_power() const;
		void directional_light_set_power(float power);
		tz::vec3 directional_light_get_colour() const;
		void directional_light_set_colour(tz::vec3 colour);

		void add_string(std::size_t string_uid, tz::vec2 pos, float size, std::string str, tz::vec3 colour);
		void remove_string(std::size_t string_uid);
		void clear_strings();
		void string_set_position(std::size_t string_uid, tz::vec2 pos);
		std::vector<std::size_t> get_all_string_uids() const;

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
		tz::gl::image_output pixelate_input;
		tz::ren::animation_renderer renderer;
		tz::ren::text_renderer text_renderer;
		tz::ren::text_renderer::font_handle default_font = tz::nullhand;
		int impl_mouse_scroll_delta = 0;
		tz::ren::animation_renderer::object_handle root = tz::nullhand;
		tz::vec2 view_bounds = {512.0f, 512.0f};
		std::vector<entry> entries = {};
		std::unordered_map<std::string, tz::ren::animation_renderer::gltf_handle> registered_models = {};
		std::unordered_map<std::string, tz::ren::animation_renderer::texture_handle> registered_textures = {};
		std::map<std::size_t, std::size_t> light_uid_to_index = {};
		std::unordered_map<std::size_t, tz::ren::text_renderer::string_handle> string_uid_to_handle = {};
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
	struct impl_rn_scene_renderer
	{
		scene_renderer* renderer = nullptr;
		int get_camera_position(tz::lua::state& state);
		int set_camera_position(tz::lua::state& state);

		int get_clear_colour(tz::lua::state& state);
		int set_clear_colour(tz::lua::state& state);

		int get_ambient_light(tz::lua::state& state);
		int set_ambient_light(tz::lua::state& state);

		int add_texture(tz::lua::state& state);
		int add_model(tz::lua::state& state);

		int get_view_bounds(tz::lua::state& state);

		int directional_light_get_direction(tz::lua::state& state);
		int directional_light_set_direction(tz::lua::state& state);
		int directional_light_get_power(tz::lua::state& state);
		int directional_light_set_power(tz::lua::state& state);
		int directional_light_get_colour(tz::lua::state& state);
		int directional_light_set_colour(tz::lua::state& state);

		int add_light(tz::lua::state& state);
		int remove_light(tz::lua::state& state);
		int light_set_position(tz::lua::state& state);
		int light_set_colour(tz::lua::state& state);
		int light_set_power(tz::lua::state& state);
		int clear_lights(tz::lua::state& state);

		int add_string(tz::lua::state& state);
		int remove_string(tz::lua::state& state);
		int clear_strings(tz::lua::state& state);
		int string_set_position(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(impl_rn_scene_renderer)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(impl_rn_scene_renderer, get_camera_position)
			LUA_METHOD(impl_rn_scene_renderer, set_camera_position)

			LUA_METHOD(impl_rn_scene_renderer, get_clear_colour)
			LUA_METHOD(impl_rn_scene_renderer, set_clear_colour)

			LUA_METHOD(impl_rn_scene_renderer, set_ambient_light)

			LUA_METHOD(impl_rn_scene_renderer, add_texture)
			LUA_METHOD(impl_rn_scene_renderer, add_model)

			LUA_METHOD(impl_rn_scene_renderer, get_view_bounds)

			LUA_METHOD(impl_rn_scene_renderer, directional_light_get_direction)
			LUA_METHOD(impl_rn_scene_renderer, directional_light_set_direction)
			LUA_METHOD(impl_rn_scene_renderer, directional_light_get_power)
			LUA_METHOD(impl_rn_scene_renderer, directional_light_set_power)
			LUA_METHOD(impl_rn_scene_renderer, directional_light_get_colour)
			LUA_METHOD(impl_rn_scene_renderer, directional_light_set_colour)

			LUA_METHOD(impl_rn_scene_renderer, add_light)
			LUA_METHOD(impl_rn_scene_renderer, remove_light)
			LUA_METHOD(impl_rn_scene_renderer, light_set_position)
			LUA_METHOD(impl_rn_scene_renderer, light_set_colour)
			LUA_METHOD(impl_rn_scene_renderer, light_set_power)
			LUA_METHOD(impl_rn_scene_renderer, clear_lights)

			LUA_METHOD(impl_rn_scene_renderer, add_string)
			LUA_METHOD(impl_rn_scene_renderer, remove_string)
			LUA_METHOD(impl_rn_scene_renderer, clear_strings)
			LUA_METHOD(impl_rn_scene_renderer, string_set_position)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END
}

#endif // RN_GAMELIB_RENDERER_SCENE_RENDERER_HPP
