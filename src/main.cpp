#include "tz/core/tz.hpp"
#include "tz/gl/renderer.hpp"
#include "tz/gl/device.hpp"
#include "tz/gl/imported_shaders.hpp"

#include ImportedShaderHeader(default, vertex)
#include ImportedShaderHeader(default, fragment)

int main()
{
	tz::initialise({.name = "Red Nightmare"});
	{
		tz::gl::RendererInfo rinfo;
		rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(default, vertex));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(default, fragment));
		tz::gl::Renderer renderer = tz::gl::device().create_renderer(rinfo);

		while(!tz::window().is_close_requested())
		{
			tz::window().begin_frame();
			renderer.render();
			tz::window().end_frame();
		}
	}
	tz::terminate();
}
