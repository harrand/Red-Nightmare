#ifndef RNLIB_RENDER_IMAGE_HPP
#define RNLIB_RENDER_IMAGE_HPP
#include "tz/gl/resource.hpp"
#include <string_view>

namespace rnlib
{
	// safe to be called from any thread... in theory.
	tz::gl::image_resource load_image_data(std::string_view img_file_data);

	using image_id_t = std::uint32_t;
	namespace image_id
	{
		enum image_id_e : image_id_t
		{
			undefined,
			invisible,
			race_human_base_idle0,
			race_human_base_idle1,
			race_human_base_side0,
			race_human_base_side1,
			race_human_base_side2,
			race_human_base_side3,
			race_human_base_up0,
			race_human_base_up1,
			race_human_base_up2,
			race_human_base_up3,
			race_human_base_down0,
			race_human_base_down1,
			race_human_base_down2,
			race_human_base_down3,
			race_human_hair_long_idle0,
			race_human_hair_long_idle1,
			race_human_hair_long_side0,
			race_human_hair_long_side1,
			race_human_hair_long_side2,
			race_human_hair_long_side3,
			race_human_hair_long_up0,
			race_human_hair_long_up1,
			race_human_hair_long_up2,
			race_human_hair_long_up3,
			race_human_hair_long_down0,
			race_human_hair_long_down1,
			race_human_hair_long_down2,
			race_human_hair_long_down3,
			race_human_hair_lord_idle0,
			race_human_hair_lord_idle1,
			race_human_hair_lord_side0,
			race_human_hair_lord_side1,
			race_human_hair_lord_side2,
			race_human_hair_lord_side3,
			race_human_hair_prince_idle0,
			race_human_hair_prince_idle1,
			race_human_hair_prince_side0,
			race_human_hair_prince_side1,
			race_human_hair_prince_side2,
			race_human_hair_prince_side3,
			race_human_hair_prince_up0,
			race_human_hair_prince_up1,
			race_human_hair_prince_up2,
			race_human_hair_prince_up3,
			race_human_hair_prince_down0,
			race_human_hair_prince_down1,
			race_human_hair_prince_down2,
			race_human_hair_prince_down3,
			race_human_eyes_dot_idle0,
			race_human_eyes_dot_idle1,
			race_human_eyes_dot_side0,
			race_human_eyes_dot_side1,
			race_human_eyes_dot_side2,
			race_human_eyes_dot_side3,
			race_human_eyes_dot_down0,
			race_human_eyes_dot_down1,
			race_human_eyes_dot_down2,
			race_human_eyes_dot_down3,
			race_human_equipment_chest_elementalrobes_idle0,
			race_human_equipment_chest_elementalrobes_idle1,
			race_human_equipment_chest_elementalrobes_side0,
			race_human_equipment_chest_elementalrobes_side1,
			race_human_equipment_chest_elementalrobes_side2,
			race_human_equipment_chest_elementalrobes_side3,
			race_human_equipment_chest_shirt_idle0,
			race_human_equipment_chest_shirt_idle1,
			race_human_equipment_chest_shirt_side0,
			race_human_equipment_chest_shirt_side1,
			race_human_equipment_chest_shirt_side2,
			race_human_equipment_chest_shirt_side3,
			race_human_equipment_chest_warrior_idle0,
			race_human_equipment_chest_warrior_idle1,
			race_human_equipment_chest_warrior_side0,
			race_human_equipment_chest_warrior_side1,
			race_human_equipment_chest_warrior_side2,
			race_human_equipment_chest_warrior_side3,
			race_human_equipment_feet_dot_idle0,
			race_human_equipment_feet_dot_side0,
			race_human_equipment_feet_dot_side1,
			race_human_equipment_feet_dot_side2,
			race_human_equipment_feet_dot_side3,
			race_human_equipment_feet_dot_up0,
			race_human_equipment_feet_dot_up1,
			race_human_equipment_feet_dot_up2,
			race_human_equipment_feet_dot_up3,
			race_human_equipment_feet_dot_down0,
			race_human_equipment_feet_dot_down1,
			race_human_equipment_feet_dot_down2,
			race_human_equipment_feet_dot_down3,
			_count
		};
	}

	tz::gl::image_resource create_image(image_id_t iid);
}

#endif // RNLIB_RENDER_IMAGE_HPP
