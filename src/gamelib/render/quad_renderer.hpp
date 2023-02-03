#ifndef RNLIB_QUAD_RENDERER_HPP
#define RNLIB_QUAD_RENDERER_HPP
#include "tz/core/data/vector.hpp"
#include "tz/gl/renderer.hpp"
#include "tz/gl/resource.hpp"

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
		void render();

		// shader data for each quad.
		struct quad_data
		{
			tz::vec2 pos = tz::vec2::zero();
			tz::vec2 scale = tz::vec2::zero();
			std::uint32_t texid = 0u;
			float pad[3];
		};
		// access data that shader sees.
		std::span<quad_data> quads();
		std::span<const quad_data> quads() const;
	private:
		// get the quad data gpu buffer.
		tz::gl::buffer_resource& quad_buffer();
		const tz::gl::buffer_resource& quad_buffer() const;

		tz::gl::resource_handle quad_bh = tz::nullhand;
		tz::gl::renderer_handle rh;
	};
}

#endif // RNLIB_QUAD_RENDERER_HPP
