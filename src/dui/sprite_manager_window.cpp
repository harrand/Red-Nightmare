#include "dui/sprite_manager_window.hpp"

namespace rn::dui
{
    SpriteManagerWindow::SpriteManagerWindow(const rn::SpriteTextureStorage& storage): tz::dui::DebugWindow("Sprite Manager"), storage(storage)
    {

    }

    void SpriteManagerWindow::render()
    {
        ImGui::Begin("Sprite Manager", &this->visible);
        std::vector<const char*> sprite_names;
        this->storage.enum_sprite_names(sprite_names);
        if(ImGui::CollapsingHeader("Sprites"))
        {
            for(const char* name : sprite_names)
            {
                ImGui::Indent();
                rn::Sprite cur_sprite = this->storage.get(name);
                if(ImGui::TreeNode(name))
                {
                    // Now get each state
                    for(std::size_t state_id = 0; state_id < static_cast<std::size_t>(rn::SpriteState::Size); state_id++)
                    {
                        ImGui::Indent();
                        auto state = static_cast<rn::SpriteState>(state_id);
                        cur_sprite.set_state(state);
                        tz::gl::BindlessTextureHandle bindless_handle = cur_sprite.get_texture();
                        ImGui::Text("\"%s\" state", rn::get_sprite_state_name(state));
                        tz::gl::dui_draw_bindless(bindless_handle);
                        ImGui::Unindent();
                    }
                    ImGui::TreePop();
                }
                ImGui::Unindent();
            }
        }
        ImGui::End();
    }
}