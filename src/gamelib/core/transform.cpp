#include "gamelib/core/transform.hpp"
#include "tz/dbgui/dbgui.hpp"

namespace rnlib
{
	box transform_t::get_bounding_box() const
	{
		auto min = tz::vec2::filled(-0.5f);
		auto max = tz::vec2::filled(0.5f);
		tz::vec2 sc = this->scale;
		sc[0] = std::abs(sc[0]);
		sc[1] = std::abs(sc[1]);
		min[0] *= sc[0];
		min[1] *= sc[1];
		max[0] *= sc[0];
		max[1] *= sc[1];
		min += this->position;
		max += this->position;
		return {min, max};
	}

	void transform_t::dbgui()
	{
		ImGui::InputFloat2("Local Position", this->position.data().data());
		ImGui::InputFloat2("Local sc", this->scale.data().data());
		ImGui::SliderFloat("Local Rotation", &this->rotation, -3.14159f, 3.14159f);
	}

	transform_t transform_t::operator+(const transform_t& rhs) const
	{
		transform_t cpy = *this;
		cpy.position += rhs.position;
		cpy.scale[0] *= rhs.scale[0];
		cpy.scale[1] *= rhs.scale[1];
		cpy.rotation += rhs.rotation;
		return cpy;
	}
}
