#ifndef RNLIB_QUAD_RENDERER_HPP
#define RNLIB_QUAD_RENDERER_HPP
#include "tz/gl/renderer.hpp"

namespace rnlib
{
	class quad_renderer
	{
	public:
		quad_renderer();
		void render();
	private:
		tz::gl::renderer_handle rh;
	};
}

#endif // RNLIB_QUAD_RENDERER_HPP
