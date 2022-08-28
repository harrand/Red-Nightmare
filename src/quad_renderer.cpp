#include "quad_renderer.hpp"
#include "tz/gl/imported_shaders.hpp"
#include "tz/gl/device.hpp"
#include "tz/dbgui/dbgui.hpp"

#include ImportedShaderHeader(quad, vertex)
#include ImportedShaderHeader(quad, fragment)

namespace game
{
	QuadRenderer::QuadRenderer():
	element_buffer_handle(tz::nullhand),
	renderer(make_renderer())
	{

	}

	void QuadRenderer::render()
	{
		// 2 triangles per quad.
		this->renderer.render(this->quad_count * 2);
	}

	void QuadRenderer::dbgui()
	{
		ImGui::Text("Quad Count: %zu", this->quad_count);
		static int quad_id = 0;
		if(this->quad_count > 0 && ImGui::CollapsingHeader("Quad Viewer"))
		{
			ImGui::SliderInt("Quad Id", &quad_id, 0, this->quad_count - 1);
			const QuadRenderer::ElementData& elem = this->renderer.get_resource(this->element_buffer_handle)->data_as<const QuadRenderer::ElementData>()[quad_id];
			ImGui::Text("Position: {%.2f, %.2f}", elem.position[0], elem.position[1]);
			ImGui::Text("Rotation (Radians): %.2f", elem.rotation);
			ImGui::Text("Texture ID: %u", static_cast<unsigned int>(elem.texture_id));
		}
	}

	tz::gl::Renderer QuadRenderer::make_renderer()
	{
		tz::gl::RendererInfo rinfo;

		rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(quad, vertex));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(quad, fragment));

		// A buffer to store data for each quad (element buffer).
		std::array<QuadRenderer::ElementData, QuadRenderer::max_quad_count> element_data;
		std::fill(element_data.begin(), element_data.end(), QuadRenderer::ElementData{});
		tz::gl::BufferResource element_buffer = tz::gl::BufferResource::from_many(element_data, tz::gl::ResourceAccess::DynamicFixed);
		this->element_buffer_handle = rinfo.add_resource(element_buffer);

		// Renderer stores all textures in the game.
		constexpr std::uint32_t tex_count = static_cast<std::uint32_t>(TextureID::Count);

		std::vector<tz::gl::ImageResource> resources;
		resources.reserve(tex_count);

		for(std::uint32_t i = 0; i < tex_count; i++)
		{
			TextureID texid = static_cast<TextureID>(i);
			resources.push_back(game::load_image(texid));
			rinfo.add_resource(resources.back());
		}

		return tz::gl::device().create_renderer(rinfo);
	}
}
