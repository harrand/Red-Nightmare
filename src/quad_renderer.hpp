#ifndef REDNIGHTMARE_QUAD_RENDERER_HPP
#define REDNIGHTMARE_QUAD_RENDERER_HPP
#include "tz/gl/renderer.hpp"
#include "tz/gl/resource.hpp"
#include "tz/core/matrix.hpp"
#include "images.hpp"
#include "statuseffect.tzsl"

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
			std::uint32_t status_effect_id = StatusEffect_None;
			float pad0;
		};

		std::span<const ElementData> elements() const;
		std::span<ElementData> elements();

		void push();
		void pop();
		void clear();
		void erase(std::size_t elem);
		float get_width_multiplier() const;
	private:
		struct RenderData
		{
			tz::Mat4 projection;
		};
		tz::gl::Renderer make_renderer();
		void update_render_data();

		static constexpr std::size_t max_quad_count = 2048;
		std::size_t quad_count = 0;
		// Stores element data for quads.
		tz::gl::ResourceHandle element_buffer_handle;
		// Stores whole scene data, such as MVP matrix components.
		tz::gl::ResourceHandle render_buffer_handle;
		tz::gl::Renderer renderer;
	};
}

#endif // REDNIGHTMARE_QUAD_RENDERER_HPP
