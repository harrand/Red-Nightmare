#include "gamelib/core/transform.hpp"

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
}
