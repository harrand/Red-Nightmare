#include "quad_renderer.hpp"
#include "tz/core/matrix_transform.hpp"
#include "tz/gl/imported_shaders.hpp"
#include "tz/gl/device.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "hdk/profile.hpp"

#include ImportedShaderHeader(quad, vertex)
#include ImportedShaderHeader(quad, fragment)

namespace game
{
	QuadRenderer::QuadRenderer():
	element_buffer_handle(hdk::nullhand),
	render_buffer_handle(hdk::nullhand),
	rendererh(this->make_renderer())
	{
		this->update_render_data();
		for(std::size_t i = 0; i < static_cast<std::size_t>(OverlayID::Count); i++)
		{
			this->push();
		}
	}

	void QuadRenderer::render()
	{
		HDK_PROFZONE("QuadRenderer - Render", 0xFF00AA00);
		this->update_render_data();
		// 2 triangles per quad.
		tz::gl::device().get_renderer(this->rendererh).render(this->quad_count * 2);
	}

	void QuadRenderer::dbgui()
	{
		ImGui::Text("Quad Count: %zu", this->quad_count);
		static bool wireframe_mode = false;
		if(ImGui::Checkbox("Wireframe Mode", &wireframe_mode))
		{
			tz::gl::device().get_renderer(this->rendererh).edit(tz::gl::RendererEditBuilder{}
			.render_state
			({
				.wireframe_mode = wireframe_mode
			}).build());
		}
		static int quad_id = 0;
		if(this->quad_count > 0)
		{
			ImGui::SliderInt("Quad Id", &quad_id, 0, this->quad_count - 1);
			QuadRenderer::ElementData& elem = this->elements()[quad_id];
			ImGui::DragFloat2("Position", elem.position.data().data(), 0.01f, -1.0f, 1.0f);
			//ImGui::Text("Rotation (Radians): %.2f", elem.rotation);
			ImGui::DragFloat2("Scale", elem.scale.data().data(), 0.01f, -1.0f, 1.0f);
			ImGui::Text("Texture ID: %u", static_cast<unsigned int>(elem.texture_id));
			ImGui::DragFloat("Layer", &elem.layer, 0.005f, -2.0f, 2.0f);
		}
	}

	const QuadRenderer::ElementData& QuadRenderer::overlay(OverlayID ovlid) const
	{
		return tz::gl::device().get_renderer(this->rendererh).get_resource(this->element_buffer_handle)->data_as<const QuadRenderer::ElementData>()[static_cast<std::size_t>(ovlid)];
	}

	QuadRenderer::ElementData& QuadRenderer::overlay(OverlayID ovlid)
	{
		return tz::gl::device().get_renderer(this->rendererh).get_resource(this->element_buffer_handle)->data_as<QuadRenderer::ElementData>()[static_cast<std::size_t>(ovlid)];
	}

	std::span<const QuadRenderer::ElementData> QuadRenderer::elements() const
	{
		return tz::gl::device().get_renderer(this->rendererh).get_resource(this->element_buffer_handle)->data_as<const QuadRenderer::ElementData>().subspan(static_cast<std::size_t>(OverlayID::Count), this->quad_count - static_cast<std::size_t>(OverlayID::Count));
	}

	std::span<QuadRenderer::ElementData> QuadRenderer::elements()
	{
		return tz::gl::device().get_renderer(this->rendererh).get_resource(this->element_buffer_handle)->data_as<QuadRenderer::ElementData>().subspan(static_cast<std::size_t>(OverlayID::Count), this->quad_count - static_cast<std::size_t>(OverlayID::Count));
	}

	void QuadRenderer::set_effect(EffectID effect, std::size_t effect_number)
	{
		hdk::assert(effect != EffectID::Count, "Cannot set effect to EffectID::Count");
		auto oid = static_cast<OverlayID>(static_cast<int>(OverlayID::Effect) + effect_number);
		if(effect == EffectID::None)
		{
			this->overlay(oid).texture_id = TextureID::Invisible;
		}
		auto eid = static_cast<std::size_t>(effect);
		eid += static_cast<std::size_t>(TextureID::Count) - 1;
		this->overlay(oid).texture_id = static_cast<TextureID>(eid);
	}

	EffectID QuadRenderer::get_effect(std::size_t effect_number) const
	{
		auto oid = static_cast<OverlayID>(static_cast<int>(OverlayID::Effect) + effect_number);
		if(this->overlay(oid).texture_id == TextureID::Invisible)
		{
			return EffectID::None;
		}
		auto eid = static_cast<std::size_t>(this->overlay(oid).texture_id);
		eid += 1;
		constexpr auto texcount = static_cast<std::size_t>(TextureID::Count);
		hdk::assert(eid >= texcount);
		eid -= texcount;
		hdk::assert(eid < static_cast<std::size_t>(EffectID::Count));
		return static_cast<EffectID>(eid);
	}
	
	float QuadRenderer::get_ambient_lighting() const
	{
		return this->ambient_lighting;
	}

	void QuadRenderer::set_ambient_lighting(float lighting)
	{
		this->ambient_lighting = lighting;
	}

	void QuadRenderer::push()
	{
		hdk::assert(this->quad_count < QuadRenderer::max_quad_count, "Ran out of quad storage in QuadRenderer.");
		tz::gl::device().get_renderer(this->rendererh).get_resource(this->element_buffer_handle)->data_as<QuadRenderer::ElementData>()[this->quad_count++] = {};
	}

	void QuadRenderer::pop()
	{
		hdk::assert(this->quad_count > static_cast<std::size_t>(OverlayID::Count), "Cannot pop when there are already no quads");
		this->quad_count--;
	}
	
	void QuadRenderer::clear()
	{
		for(auto& elem : this->elements())
		{
			elem = QuadRenderer::ElementData{};
		}
		this->quad_count = static_cast<std::size_t>(OverlayID::Count);
	}

	void QuadRenderer::erase(std::size_t id)
	{
		hdk::assert(id < this->elements().size(), "Invalid Quad ID");
		std::swap(this->elements()[id], this->elements().back());
		this->pop();
	}

	float QuadRenderer::get_width_multiplier() const
	{
		return static_cast<float>(tz::window().get_width()) / tz::window().get_height();
	}

	const hdk::vec2& QuadRenderer::camera_position() const
	{
		return this->camera_pos;
	}

	hdk::vec2& QuadRenderer::camera_position()
	{
		return this->camera_pos;
	}

	float QuadRenderer::get_camera_zoom() const
	{
		return this->camera_zoom;
	}
	
	void QuadRenderer::set_camera_zoom(float zoom)
	{
		this->camera_zoom = zoom;
	}

	tz::gl::RendererHandle QuadRenderer::make_renderer()
	{
		tz::gl::RendererInfo rinfo;

		rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(quad, vertex));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(quad, fragment));
		rinfo.state().graphics.clear_colour = {0.0f, 0.0f, 0.0f, 1.0f};

		// A buffer to store data for each quad (element buffer).
		std::array<QuadRenderer::ElementData, QuadRenderer::max_quad_count> element_data;
		std::fill(element_data.begin(), element_data.end(), QuadRenderer::ElementData{});
		tz::gl::BufferResource element_buffer = tz::gl::BufferResource::from_many(element_data,
		{
			.access = tz::gl::ResourceAccess::DynamicFixed
		});

		tz::gl::BufferResource render_buffer = tz::gl::BufferResource::from_one(QuadRenderer::RenderData{},
		{
			.access = tz::gl::ResourceAccess::DynamicFixed
		});
		this->element_buffer_handle = rinfo.add_resource(element_buffer);
		this->render_buffer_handle = rinfo.add_resource(render_buffer);
		rinfo.ref_resource(game::effects().get_point_light_buffer());

		// Renderer stores all textures in the game.
		constexpr std::uint32_t tex_count = static_cast<std::uint32_t>(TextureID::Count);

		for(std::uint32_t i = 0; i < tex_count; i++)
		{
			rinfo.add_resource(game::load_image(static_cast<TextureID>(i)));
		}

		// Effect images come afterwards.
		for(std::size_t i = 0; i < static_cast<std::size_t>(EffectID::Count); i++)
		{
			auto* comp = game::effects().get_effect_component(static_cast<EffectID>(i));
			if(comp == nullptr)
			{
				continue;
			}
			rinfo.ref_resource(comp);
		}

		return tz::gl::device().create_renderer(rinfo);
	}

	void QuadRenderer::update_render_data()
	{
		HDK_PROFZONE("QuadRenderer - Update Render Data", 0xFF8B4513);
		RenderData& data = tz::gl::device().get_renderer(this->rendererh).get_resource(this->render_buffer_handle)->data_as<RenderData>().front();
		const float aspect_ratio = this->get_width_multiplier();
		const float cam_mult = this->camera_zoom;
		data =
		{
			.view = tz::view(this->camera_pos.with_more(0.0f), hdk::vec3{0.0f, 0.0f, 0.0f}),
			.projection = tz::orthographic(-aspect_ratio * cam_mult, aspect_ratio * cam_mult, 1.0f * cam_mult, -1.0f * cam_mult, 0.0f, -1.0f),
			.ambient_lighting = this->ambient_lighting
		};
	}
}
