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
			tz::Vec2 scale = {0.2f, 0.2f};
			float rotation = 0.0f;
			TextureID texture_id = static_cast<TextureID>(0);
		};

		std::span<const ElementData> elements() const;
		std::span<ElementData> elements();

		void push();
		void pop();
		void clear();
	private:
		tz::gl::Renderer make_renderer();

		static constexpr std::size_t max_quad_count = 2048;
		std::size_t quad_count = 0;
		tz::gl::ResourceHandle element_buffer_handle;
		tz::gl::Renderer renderer;
	};
}

#endif // REDNIGHTMARE_QUAD_RENDERER_HPP
