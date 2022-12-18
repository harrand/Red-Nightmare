#ifndef REDNIGHTMARE_QUAD_RENDERER_HPP
#define REDNIGHTMARE_QUAD_RENDERER_HPP
#include "tz/gl/renderer.hpp"
#include "tz/gl/resource.hpp"
#include "tz/core/matrix.hpp"
#include "images.hpp"
#include "statuseffect.tzsl"

namespace game
{
	enum class OverlayID
	{
		Backdrop,
		Count
	};

	class QuadRenderer
	{
	public:
		QuadRenderer();

		void render();
		void dbgui();

		struct ElementData
		{
			hdk::vec2 position = {0.0f, 0.0f};
			hdk::vec2 scale = {0.2f, 0.2f};
			float rotation = 0.0f;
			TextureID texture_id = static_cast<TextureID>(0);
			std::uint32_t status_effect_id = StatusEffect_None;
			float layer = 0.0f;
		};

		const ElementData& overlay(OverlayID ovlid) const;
		ElementData& overlay(OverlayID ovlid);
		std::span<const ElementData> elements() const;
		std::span<ElementData> elements();

		void push();
		void pop();
		void clear();
		void erase(std::size_t elem);
		float get_width_multiplier() const;
		const hdk::vec2& camera_position() const;
		hdk::vec2& camera_position();
		static constexpr std::size_t max_quad_count = 2048;
	private:
		struct RenderData
		{
			tz::Mat4 view = tz::Mat4::identity();
			tz::Mat4 projection = tz::Mat4::identity();
		};
		tz::gl::RendererHandle make_renderer();
		void update_render_data();

		std::size_t quad_count = 0;
		// Stores element data for quads.
		tz::gl::ResourceHandle element_buffer_handle;
		// Stores whole scene data, such as MVP matrix components.
		tz::gl::ResourceHandle render_buffer_handle;
		tz::gl::RendererHandle rendererh;
		hdk::vec2 camera_pos{0.0f, 0.0f};
	};
}

#endif // REDNIGHTMARE_QUAD_RENDERER_HPP
