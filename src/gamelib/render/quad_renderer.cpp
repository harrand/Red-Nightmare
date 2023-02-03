#include "gamelib/render/quad_renderer.hpp"
#include "tz/gl/device.hpp"
#include "tz/gl/imported_shaders.hpp"

#include ImportedShaderHeader(empty, vertex)
#include ImportedShaderHeader(empty, fragment)

namespace rnlib
{
	quad_renderer::quad_renderer():
	rh([this]()
	{
		tz::gl::renderer_info rinfo;
		rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(empty, vertex));
		rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(empty, fragment));
		return tz::gl::get_device().create_renderer(rinfo);
	}())
	{

	}

	void quad_renderer::render()
	{
		tz::gl::get_device().get_renderer(this->rh).render(0);
	}
}
