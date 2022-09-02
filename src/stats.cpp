#include "stats.hpp"
#include "tz/dbgui/dbgui.hpp"

namespace game
{
	void Stats::dbgui() const
	{
		ImGui::Text("Movement Speed: %u%", static_cast<unsigned int>(100.0f * this->movement_speed / default_base_movement));
		constexpr float global_multiplier = 360.0f;
		ImGui::Text("Damage: %.1f", this->damage * global_multiplier);
		ImGui::Text("Max Health: %.1f", this->max_health * global_multiplier);
		ImGui::Text("Defense: %.1f", this->defense * global_multiplier);
	}

	StatBuff get_buff(BuffID buff)
	{
		switch(buff)
		{
			case BuffID::Berserk:
				return
				{
					.multiply_damage = 10.0f
				};
			break;
			case BuffID::Sprint:
				return
				{
					.multiply_speed_boost = 1.7f,
					.time_remaining_millis = 5000.0f
				};
			break;
		}
		return {};
	}
}
