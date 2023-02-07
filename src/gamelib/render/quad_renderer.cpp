#include "gamelib/render/quad_renderer.hpp"
#include "tz/core/profile.hpp"
#include "tz/gl/device.hpp"
#include "tz/gl/imported_shaders.hpp"

#include ImportedShaderHeader(quad, vertex)
#include ImportedShaderHeader(quad, fragment)

namespace rnlib
{
	constexpr std::size_t initial_capacity = 512;

	quad_renderer::quad_renderer():
	rh([this]()
	{
		TZ_PROFZONE("quad_renderer - initialise", 0xff0077ee);
		tz::gl::renderer_info rinfo;
		rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(quad, vertex));
		rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(quad, fragment));
		std::array<quad_data, initial_capacity> initial_quads;
		this->quad_bh = rinfo.add_resource(tz::gl::buffer_resource::from_many(initial_quads,
		{
			.access = tz::gl::resource_access::dynamic_variable
		}));
		this->data_bh = rinfo.add_resource(tz::gl::buffer_resource::from_one(render_data{},
		{
			.access = tz::gl::resource_access::dynamic_fixed
		}));
		return tz::gl::get_device().create_renderer(rinfo);
	}())
	{

	}

	void quad_renderer::render()
	{
		TZ_PROFZONE("quad_renderer - render", 0xffee0077);
		tz::assert(this->rh != tz::nullhand, "quad_renderer renderer handle is nullhand. initialisation failed in some weird way. submit a bug report.");
		const std::size_t tri_count = this->quads().size() * 2;
		tz::gl::get_device().get_renderer(this->rh).render(tri_count);
	}

	void quad_renderer::dbgui()
	{
		if(ImGui::CollapsingHeader("Debug Operations"))
		{
			if(ImGui::Button("Debug Clean"))
			{
				this->clean();
			}
		}
		if(this->quads().empty())
		{
			ImGui::Text("nothing being rendered.");
			return;
		}
		static int qpos = 0;
		ImGui::SliderInt("quad id", &qpos, 0, this->quads().size() - 1);
		quad_renderer::quad_data& quad = this->quads()[qpos];
		
		ImGui::Indent();
		ImGui::InputFloat2("position", quad.pos.data().data());
		ImGui::InputFloat2("scale", quad.scale.data().data());
		ImGui::DragFloat("rotation", &quad.rotation, 0.01f, -3.14159f, 3.14159f);
		ImGui::Text("texture id: %u", static_cast<unsigned int>(quad.texid));
		ImGui::DragInt("layer", reinterpret_cast<int*>(&quad.layer), 0.25f, 0, 1000, "%zu");
		ImGui::Unindent();
	}

	std::span<quad_renderer::quad_data> quad_renderer::quads()
	{
		return this->quad_buffer().data_as<quad_renderer::quad_data>();
	}

	std::span<const quad_renderer::quad_data> quad_renderer::quads() const
	{
		return this->quad_buffer().data_as<const quad_renderer::quad_data>();
	}

	void quad_renderer::clean()
	{
		auto qs = this->quads();
		std::fill(qs.begin(), qs.end(), quad_data{});
	}

	void quad_renderer::reserve(std::size_t quad_count)
	{
		if(this->quads().size() < quad_count)
		{
			// need to resize the buffer.
			tz::gl::get_device().get_renderer(this->rh).edit
			(
				tz::gl::RendererEditBuilder{}
				.buffer_resize({.buffer_handle = this->quad_bh, .size = sizeof(quad_renderer::quad_data) * quad_count})
				.build()
			);
		}
	}

	void quad_renderer::set_render_data(render_data data)
	{
		this->render_buffer().data_as<render_data>().front() = data;
	}

	tz::gl::buffer_resource& quad_renderer::quad_buffer()
	{
		return *static_cast<tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->quad_bh));
	}

	const tz::gl::buffer_resource& quad_renderer::quad_buffer() const
	{
		return *static_cast<const tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->quad_bh));
	}

	tz::gl::buffer_resource& quad_renderer::render_buffer()
	{
		return *static_cast<tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->data_bh));
	}

	const tz::gl::buffer_resource& quad_renderer::render_buffer() const
	{
		return *static_cast<const tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->data_bh));
	}
}
