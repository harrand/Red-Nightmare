#ifndef REDNIGHTMARE_QUAD_RENDERER_HPP
#define REDNIGHTMARE_QUAD_RENDERER_HPP
#include "tz/gl/renderer.hpp"
#include "tz/gl/resource.hpp"
#include "tz/core/matrix.hpp"
#include "images.hpp"
#include "effect.hpp"
#include "statuseffect.tzsl"

namespace game
{
	enum class OverlayID
	{
		Backdrop_Background,
		Backdrop_Foreground,
		Effect,
		Effect1,
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
			tz::vec2 position = {0.0f, 0.0f};
			tz::vec2 scale = {0.2f, 0.2f};
			TextureID texture_id = static_cast<TextureID>(0);
			TextureID normal_map_id = TextureID::Smooth_Normals;
			std::uint32_t status_effect_id = StatusEffect_None;
			float layer = 0.0f;
			tz::vec2 texcoord_scale = {1.0f, 1.0f};
		};

		const ElementData& overlay(OverlayID ovlid) const;
		ElementData& overlay(OverlayID ovlid);
		std::span<const ElementData> elements() const;
		std::span<ElementData> elements();
		void set_effect(EffectID effect, std::size_t effect_number = 0);
		EffectID get_effect(std::size_t effect_number = 0) const;
		float get_ambient_lighting() const;
		void set_ambient_lighting(float lighting);

		void push();
		void pop();
		void clear();
		void erase(std::size_t elem);
		float get_width_multiplier() const;
		const tz::vec2& camera_position() const;
		tz::vec2& camera_position();
		float get_camera_zoom() const;
		void set_camera_zoom(float zoom);
		static constexpr std::size_t max_quad_count = 8096;
	private:
		struct RenderData
		{
			tz::mat4 view = tz::mat4::identity();
			tz::mat4 projection = tz::mat4::identity();
			float ambient_lighting = 1.0f;
		};
		tz::gl::renderer_handle make_renderer();
		void update_render_data();

		std::size_t quad_count = 0;
		// Stores element data for quads.
		tz::gl::resource_handle element_buffer_handle;
		// Stores whole scene data, such as MVP matrix components.
		tz::gl::resource_handle render_buffer_handle;
		tz::gl::renderer_handle rendererh;
		tz::vec2 camera_pos{0.0f, 0.0f};
		float ambient_lighting = 1.0f;
		float camera_zoom = 1.0f;
	};
}

#endif // REDNIGHTMARE_QUAD_RENDERER_HPP
