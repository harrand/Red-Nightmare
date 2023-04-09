#include "gamelib/render/quad_renderer.hpp"
#include "gamelib/render/image.hpp"
#include "tz/core/profile.hpp"
#include "tz/gl/device.hpp"
#include "tz/gl/draw.hpp"
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
		rinfo.set_options({tz::gl::renderer_option::no_present});
		// quad buffer
		std::array<quad_data, initial_capacity> initial_quads;
		this->quad_bh = rinfo.add_resource(tz::gl::buffer_resource::from_many(initial_quads,
		{
			.access = tz::gl::resource_access::dynamic_variable
		}));
		// data buffer (e.g camera)
		this->data_bh = rinfo.add_resource(tz::gl::buffer_resource::from_one(render_data{},
		{
			.access = tz::gl::resource_access::dynamic_fixed
		}));
		// indirect draw buffer
		this->indirect_bh = rinfo.add_resource(tz::gl::buffer_resource::from_one(tz::gl::draw_indirect_command
		{
			.count = 0u,
			.first = 0
		},
		{
			.access = tz::gl::resource_access::dynamic_fixed,
			.flags = {tz::gl::resource_flag::draw_indirect_buffer}
		}));
		rinfo.state().graphics.draw_buffer = this->indirect_bh;
		// all images
		for(image_id_t i = 0; i < image_id::_count; i++)
		{
			rinfo.add_resource(rnlib::create_image(i));
		}
		return tz::gl::get_device().create_renderer(rinfo);
	}())
	{

	}

	void quad_renderer::render()
	{
		TZ_PROFZONE("quad_renderer - render", 0xffee0077);
		tz::assert(this->rh != tz::nullhand, "quad_renderer renderer handle is nullhand. initialisation failed in some weird way. submit a bug report.");
		this->indirect_buffer().data_as<tz::gl::draw_indirect_command>().front().count = this->quads().size() * 2 * 3;
	}

	void quad_renderer::dbgui()
	{
		if(ImGui::BeginTabBar("Quad Renderer"))
		{
			if(ImGui::BeginTabItem("Quad Data"))
			{
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
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Debug View Options"))
			{
				render_data& rdata = this->get_render_data();
				static int view_opt = 0;
				ImGui::RadioButton("textured", &view_opt, 0x000);
				ImGui::RadioButton("coloured", &view_opt, 0x001);
				ImGui::RadioButton("depth", &view_opt, 0x010);
				rdata.debug_depth_view = (view_opt & 0x010);
				rdata.debug_colour_view = (view_opt & 0x001);
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Debug Operations"))
			{
				if(ImGui::Button("Debug Clean"))
				{
					this->clean();
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}

	tz::gl::renderer_handle quad_renderer::get() const
	{
		return this->rh;
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
		// preserve debug data.
		auto& d = this->get_render_data();
		bool debug_depth_view_cache = d.debug_depth_view;
		bool debug_colour_view_cache = d.debug_colour_view;
		d = data;
		d.debug_depth_view = debug_depth_view_cache;
		d.debug_colour_view = debug_colour_view_cache;
	}

	quad_renderer::render_data& quad_renderer::get_render_data()
	{
		return this->render_buffer().data_as<render_data>().front();
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

	tz::gl::buffer_resource& quad_renderer::indirect_buffer()
	{
		return *static_cast<tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->indirect_bh));
	}
}
