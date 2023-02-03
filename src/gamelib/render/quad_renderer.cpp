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
		tz::gl::get_device().get_renderer(this->rh).render(0);
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
