#include "player_manager_window.hpp"

namespace rn::dui
{
    PlayerManagerWindow::PlayerManagerWindow(PlayerSkin& skin): tz::dui::DebugWindow("Player Manager"), skin(skin){}

    void PlayerManagerWindow::render()
    {
        ImGui::Begin("Player Manager");
        ImGui::Text("Player Skin:");
        static int c = 0;
        ImGui::RadioButton("Ancient Empire", &c, static_cast<int>(PlayerSkin::Default));
        ImGui::RadioButton("Hokenian", &c, static_cast<int>(PlayerSkin::Hokenian));
        ImGui::RadioButton("Anthir", &c, static_cast<int>(PlayerSkin::Anthir));
        this->skin = static_cast<PlayerSkin>(c);
        ImGui::End();
    }
}