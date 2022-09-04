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

	bool StatBuff::operator==(const StatBuff& rhs) const
	{
		return this->multiply_speed_boost == rhs.multiply_speed_boost
		    && this->add_speed_boost == rhs.add_speed_boost
		    && this->multiply_damage == rhs.multiply_damage
		    && this->add_damage == rhs.add_damage
		    && this->multiply_defense == rhs.multiply_defense
		    && this->add_defense == rhs.add_defense
		    && this->multiply_health == rhs.multiply_health
		    && this->add_health == rhs.add_health;
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
			case BuffID::Chill:
				return
				{
					.multiply_speed_boost = 0.3f,
					.time_remaining_millis = 5000.0f
				};
			break;
		}
		return {};
	}

	void StatBuffs::update()
	{

	}

	void StatBuffs::add(BuffID buff)
	{
		this->buffs.push_back(game::get_buff(buff));
	}

	bool StatBuffs::contains(BuffID buff) const
	{
		return std::find(this->buffs.begin(), this->buffs.end(), game::get_buff(buff)) != this->buffs.end();
	}

	std::span<const StatBuff> StatBuffs::elements() const
	{
		return this->buffs;
	}

	std::span<StatBuff> StatBuffs::elements()
	{
		return this->buffs;
	}
}
