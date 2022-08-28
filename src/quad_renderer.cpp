#include "quad_renderer.hpp"
#include "tz/gl/imported_shaders.hpp"
#include "tz/gl/device.hpp"

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

	tz::gl::Renderer QuadRenderer::make_renderer()
	{
		tz::gl::RendererInfo rinfo;

		rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(quad, vertex));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(quad, fragment));

		// A buffer to store data for each quad (element buffer).
		std::vector<QuadRenderer::ElementData> element_data;
		element_data.resize(this->quad_count, {});
		tz::gl::BufferResource element_buffer = tz::gl::BufferResource::from_many(element_data, tz::gl::ResourceAccess::DynamicVariable);
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
