#ifndef RNLIB_QUAD_RENDERER_HPP
#define RNLIB_QUAD_RENDERER_HPP
#include "tz/core/data/vector.hpp"
#include "tz/core/matrix.hpp"
#include "tz/gl/renderer.hpp"
#include "tz/gl/resource.hpp"
#include "gamelib/render/shaders/quadtex.tzsl"

namespace rnlib
{
	// responsible for rendering textured quads in various positions and scales. if it's ever involved in anything more i have bloated this class and should be slapped.
	class quad_renderer
	{
	public:
		quad_renderer();
		quad_renderer(const quad_renderer& cpy) = delete;
		quad_renderer(quad_renderer&& mv) = delete;
		~quad_renderer() = default;
		quad_renderer& operator=(const quad_renderer& rhs) = delete;
		quad_renderer& operator=(quad_renderer&& rhs) = delete;

		// draw all them quads.
		void render(std::size_t quad_count);
		void dbgui();

		tz::gl::renderer_handle get() const;

		// shader data for each quad.
		struct quad_data
		{
			tz::vec2 pos = tz::vec2::zero();
			tz::vec2 scale = tz::vec2::zero();
			float rotation = 0.0f;
			std::array<std::uint32_t, QUAD_TEXCOUNT> texid;
			float pad0[3];
			std::array<tz::vec4, QUAD_TEXCOUNT> tints;
			std::int32_t layer = 0;
			float pad1[3];
		};

		struct render_data
		{
			tz::mat4 view = tz::mat4::identity();
			tz::mat4 projection = tz::mat4::identity();
			std::uint32_t debug_depth_view = false;
			std::uint32_t debug_colour_view = false;
		};
		// access data that shader sees.
		std::span<quad_data> quads();
		std::span<const quad_data> quads() const;
		// set all quads to default, aka clear screen.
		void clean();
		void reserve(std::size_t quad_count);
		void set_render_data(render_data data);
	private:
		render_data& get_render_data();
		// get the quad data gpu buffer.
		tz::gl::buffer_resource& quad_buffer();
		const tz::gl::buffer_resource& quad_buffer() const;
		tz::gl::buffer_resource& render_buffer();
		const tz::gl::buffer_resource& render_buffer() const;
		tz::gl::buffer_resource& indirect_buffer();

		tz::gl::resource_handle quad_bh = tz::nullhand;
		tz::gl::resource_handle data_bh = tz::nullhand;
		tz::gl::resource_handle indirect_bh = tz::nullhand;
		tz::gl::renderer_handle rh;
	};
}

#endif // RNLIB_QUAD_RENDERER_HPP
