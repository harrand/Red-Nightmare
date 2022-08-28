#ifndef REDNIGHTMARE_QUAD_RENDERER_HPP
#define REDNIGHTMARE_QUAD_RENDERER_HPP
#include "tz/gl/renderer.hpp"
#include "tz/gl/resource.hpp"
#include "images.hpp"

namespace game
{
	class QuadRenderer
	{
	public:
		QuadRenderer();

		void render();
		void dbgui();

		struct ElementData
		{
			tz::Vec2 position = {0.0f, 0.0f};
			float rotation = 0.0f;
			TextureID texture_id = static_cast<TextureID>(0);
		};
	private:
		tz::gl::Renderer make_renderer();

		std::size_t quad_count = 1;
		tz::gl::ResourceHandle element_buffer_handle;
		tz::gl::Renderer renderer;
	};
}

#endif // REDNIGHTMARE_QUAD_RENDERER_HPP
