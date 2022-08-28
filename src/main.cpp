#include "quad_renderer.hpp"
#include "animation.hpp"
#include "tz/core/tz.hpp"
#include "tz/dbgui/dbgui.hpp"


int main()
{
	tz::initialise({.name = "Red Nightmare"});
	{
		game::QuadRenderer qrenderer;
		game::Animation idle = game::play_animation(game::AnimationID::PlayerClassic_MoveSide);

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

			qrenderer.elements().front().texture_id = idle.get_texture();
			tz::window().end_frame();
		}
	}
	tz::terminate();
}
