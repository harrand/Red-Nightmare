#include "quad_renderer.hpp"
#include "tz/core/tz.hpp"
#include "tz/dbgui/dbgui.hpp"


int main()
{
	tz::initialise({.name = "Red Nightmare"});
	{
		game::QuadRenderer qrenderer;

		bool show_game_menu = false;
		tz::dbgui::game_menu().add_callback([&show_game_menu]()
		{
			ImGui::MenuItem("Quad Renderer", nullptr, &show_game_menu);
		});

		while(!tz::window().is_close_requested())
		{
			tz::window().begin_frame();
			qrenderer.render();

			tz::dbgui::run([&show_game_menu, &qrenderer]()
			{
				if(show_game_menu)
				{
					ImGui::Begin("Quad Renderer", &show_game_menu);
					qrenderer.dbgui();
					ImGui::End();
				}
			});

			if(qrenderer.elements().front().texture_id == game::TextureID::PlayerClassic_Idle_1)
			{
				qrenderer.elements().front().texture_id = game::TextureID::PlayerClassic_Idle_2;
			}
			else
			{
				qrenderer.elements().front().texture_id = game::TextureID::PlayerClassic_Idle_1;
			}
			tz::window().end_frame();
		}
	}
	tz::terminate();
}
