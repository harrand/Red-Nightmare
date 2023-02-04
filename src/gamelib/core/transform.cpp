#include "gamelib/core/transform.hpp"
#include "tz/dbgui/dbgui.hpp"

namespace rnlib
{
	tz::vec2 transform_t::get_position() const
	{
		tz::vec2 pos = this->local_position;
		if(this->parent != nullptr)
		{
			pos += parent->get_position();
		}
		return pos;
	}

	tz::vec2 transform_t::get_scale() const
	{
		tz::vec2 scale = this->local_scale;
		if(this->parent != nullptr)
		{
			auto pscale = parent->get_scale();
			scale[0] *= pscale[0];
			scale[1] *= pscale[1];
		}
		return scale;
	}

	float transform_t::get_rotation() const
	{
		float rot = this->local_rotation;
		if(this->parent != nullptr)
		{
			rot += this->parent->get_rotation();
		}
		return rot;
	}

	void transform_t::dbgui()
	{
		ImGui::InputFloat2("Local Position", this->local_position.data().data());
		ImGui::InputFloat2("Local Scale", this->local_scale.data().data());
		ImGui::SliderFloat("Local Rotation", &this->local_rotation, -3.14159f, 3.14159f);

		auto position = this->get_position();
		ImGui::Text("Position = {%.3f, %.3f}", position[0], position[1]);
		auto scale = this->get_scale();
		ImGui::Text("Scale = {%.3f, %.3f}", scale[0], scale[1]);
		auto rotation = this->get_rotation();
		ImGui::Text("Rotation = %.3f", rotation);
	}
}
