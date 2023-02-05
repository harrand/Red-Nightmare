#include "gamelib/render/quad_renderer.hpp"
#include "tz/core/profile.hpp"
#include "tz/gl/device.hpp"
#include "tz/gl/imported_shaders.hpp"

#include ImportedShaderHeader(quad, vertex)
#include ImportedShaderHeader(quad, fragment)

namespace rnlib
{
	constexpr std::size_t max_quads = 8096;

	quad_renderer::quad_renderer():
	rh([this]()
	{
		TZ_PROFZONE("quad_renderer - initialise", 0xff0077ee);
		tz::gl::renderer_info rinfo;
		rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(quad, vertex));
		rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(quad, fragment));
		std::array<quad_data, max_quads> initial_quads;
		this->quad_bh = rinfo.add_resource(tz::gl::buffer_resource::from_many(initial_quads,
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
		const std::size_t tri_count = this->quad_cursor * 2;
		tz::gl::get_device().get_renderer(this->rh).render(tri_count);
	}

	void quad_renderer::dbgui()
	{
		if(ImGui::CollapsingHeader("Debug Operations"))
		{
			if(ImGui::Button("Debug Push"))
			{
				this->reserve(1);
			}
			if(ImGui::Button("Debug Pop"))
			{
				this->pop(1);
			}
			if(ImGui::Button("Debug Clear"))
			{
				this->clear();
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
		ImGui::Unindent();
	}

	std::span<quad_renderer::quad_data> quad_renderer::quads()
	{
		return this->quad_buffer().data_as<quad_renderer::quad_data>().subspan(0, this->quad_cursor);
	}

	std::span<const quad_renderer::quad_data> quad_renderer::quads() const
	{
		return this->quad_buffer().data_as<const quad_renderer::quad_data>().subspan(0, this->quad_cursor);
	}

	std::size_t quad_renderer::size() const
	{
		return this->quad_cursor;
	}

	void quad_renderer::clear_quads()
	{
		auto qs = this->quads();
		std::fill(qs.begin(), qs.end(), quad_renderer::quad_data{});
	}

	void quad_renderer::reserve(std::size_t count)
	{
		tz::assert(this->quad_cursor + count < max_quads, "quad_renderer reserve of %zu quads would go over the maximum of %zu (cursor = %zu). Resizing is not yet implemented.", count, max_quads, this->quad_cursor);
		this->quad_cursor += count;
	}


	void quad_renderer::pop(std::size_t count)
	{
		tz::assert(this->quad_cursor >= count, "quad_renderer pop of %zu quads is invalid because there are only %zu quads reserved.", count, this->quad_cursor);
		this->quad_cursor -= count;
	}

	void quad_renderer::clear()
	{
		this->quad_cursor = 0;
	}

	tz::gl::buffer_resource& quad_renderer::quad_buffer()
	{
		return *static_cast<tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->quad_bh));
	}

	const tz::gl::buffer_resource& quad_renderer::quad_buffer() const
	{
		return *static_cast<const tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->quad_bh));
	}
}
