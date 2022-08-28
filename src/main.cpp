#include "tz/core/tz.hpp"
#include "tz/gl/imported_shaders.hpp"
#include "tz/gl/renderer.hpp"
#include "tz/gl/device.hpp"
#include "tz/dbgui/dbgui.hpp"

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

		bool show_game_menu = false;
		tz::dbgui::game_menu().add_callback([&show_game_menu]()
		{
			ImGui::MenuItem("Blah", nullptr, &show_game_menu);
		});

		while(!tz::window().is_close_requested())
		{
			tz::window().begin_frame();
			renderer.render();

			tz::dbgui::run([&show_game_menu]()
			{
				if(show_game_menu)
				{
					ImGui::Begin("Blah", &show_game_menu);
					ImGui::Text("Herp");
					ImGui::End();
				}
			});
			tz::window().end_frame();
		}
	}
	tz::terminate();
}
