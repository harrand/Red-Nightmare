#ifndef REDNIGHTMARE_PLAYER_MANAGER_WINDOW_HPP
#define REDNIGHTMARE_PLAYER_MANAGER_WINDOW_HPP
#include "dui/window.hpp"

constexpr char player_skin_default_name[] = "player";
constexpr char player_skin_hokenian_name[] = "player/hokenian";
constexpr char player_skin_anthir_name[] = "player/anthir";

enum class PlayerSkin
{
    Default,
    Hokenian,
    Anthir
};

namespace rn::dui
{
    class PlayerManagerWindow : public tz::dui::DebugWindow
    {
    public:
        PlayerManagerWindow(PlayerSkin& skin);
        virtual void render() override;
    private:
        PlayerSkin& skin;
    };
}

#endif // REDNIGHTMARE_PLAYER_MANAGER_WINDOW_HPP