#include "gamelib/render/image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace rnlib
{
	tz::gl::image_resource load_image_data(std::string_view img_file_data)
	{
		int w, h, channels;
		stbi_uc* imgdata = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(img_file_data.data()), img_file_data.size(), &w, &h, &channels, 4);
		std::span<stbi_uc> imgdata_span{imgdata, static_cast<std::size_t>(w * h * 4)};
		tz::gl::image_resource ret = tz::gl::image_resource::from_memory(imgdata_span,
		{
			.format = tz::gl::image_format::RGBA32,
			.dimensions = static_cast<tz::vec2ui>(tz::vec2i{w, h}),
			.flags = {tz::gl::resource_flag::image_wrap_repeat}
		});
		stbi_image_free(imgdata);
		return ret;
	}

}

// image implementations.
#include "tz/core/imported_text.hpp"
#include ImportedTextHeader(invisible, png)
#include ImportedTextHeader(undefined, png)
#include ImportedTextHeader(human_base_idle0, png)
#include ImportedTextHeader(human_base_idle1, png)
#include ImportedTextHeader(human_base_side0, png)
#include ImportedTextHeader(human_base_side1, png)
#include ImportedTextHeader(human_base_side2, png)
#include ImportedTextHeader(human_base_side3, png)
#include ImportedTextHeader(human_base_up0, png)
#include ImportedTextHeader(human_base_up1, png)
#include ImportedTextHeader(human_base_up2, png)
#include ImportedTextHeader(human_base_up3, png)
#include ImportedTextHeader(human_base_down0, png)
#include ImportedTextHeader(human_base_down1, png)
#include ImportedTextHeader(human_base_down2, png)
#include ImportedTextHeader(human_base_down3, png)
#include ImportedTextHeader(human_hair_long_idle0, png)
#include ImportedTextHeader(human_hair_long_idle1, png)
#include ImportedTextHeader(human_hair_long_side0, png)
#include ImportedTextHeader(human_hair_long_side1, png)
#include ImportedTextHeader(human_hair_long_side2, png)
#include ImportedTextHeader(human_hair_long_side3, png)
#include ImportedTextHeader(human_hair_long_up0, png)
#include ImportedTextHeader(human_hair_long_up1, png)
#include ImportedTextHeader(human_hair_long_up2, png)
#include ImportedTextHeader(human_hair_long_up3, png)
#include ImportedTextHeader(human_hair_long_down0, png)
#include ImportedTextHeader(human_hair_long_down1, png)
#include ImportedTextHeader(human_hair_long_down2, png)
#include ImportedTextHeader(human_hair_long_down3, png)
#include ImportedTextHeader(human_hair_lord_idle0, png)
#include ImportedTextHeader(human_hair_lord_idle1, png)
#include ImportedTextHeader(human_hair_lord_side0, png)
#include ImportedTextHeader(human_hair_lord_side1, png)
#include ImportedTextHeader(human_hair_lord_side2, png)
#include ImportedTextHeader(human_hair_lord_side3, png)
#include ImportedTextHeader(human_hair_lord_up0, png)
#include ImportedTextHeader(human_hair_lord_up1, png)
#include ImportedTextHeader(human_hair_lord_up2, png)
#include ImportedTextHeader(human_hair_lord_up3, png)
#include ImportedTextHeader(human_hair_lord_down0, png)
#include ImportedTextHeader(human_hair_lord_down1, png)
#include ImportedTextHeader(human_hair_lord_down2, png)
#include ImportedTextHeader(human_hair_lord_down3, png)
#include ImportedTextHeader(human_hair_prince_idle0, png)
#include ImportedTextHeader(human_hair_prince_idle1, png)
#include ImportedTextHeader(human_hair_prince_side0, png)
#include ImportedTextHeader(human_hair_prince_side1, png)
#include ImportedTextHeader(human_hair_prince_side2, png)
#include ImportedTextHeader(human_hair_prince_side3, png)
#include ImportedTextHeader(human_hair_prince_up0, png)
#include ImportedTextHeader(human_hair_prince_up1, png)
#include ImportedTextHeader(human_hair_prince_up2, png)
#include ImportedTextHeader(human_hair_prince_up3, png)
#include ImportedTextHeader(human_hair_prince_down0, png)
#include ImportedTextHeader(human_hair_prince_down1, png)
#include ImportedTextHeader(human_hair_prince_down2, png)
#include ImportedTextHeader(human_hair_prince_down3, png)
#include ImportedTextHeader(human_eyes_dot_idle0, png)
#include ImportedTextHeader(human_eyes_dot_idle1, png)
#include ImportedTextHeader(human_eyes_dot_side0, png)
#include ImportedTextHeader(human_eyes_dot_side1, png)
#include ImportedTextHeader(human_eyes_dot_side2, png)
#include ImportedTextHeader(human_eyes_dot_side3, png)
#include ImportedTextHeader(human_eyes_dot_down0, png)
#include ImportedTextHeader(human_eyes_dot_down1, png)
#include ImportedTextHeader(human_eyes_dot_down2, png)
#include ImportedTextHeader(human_eyes_dot_down3, png)
#include ImportedTextHeader(human_equipment_chest_elementalrobes_idle0, png)
#include ImportedTextHeader(human_equipment_chest_elementalrobes_idle1, png)
#include ImportedTextHeader(human_equipment_chest_elementalrobes_side0, png)
#include ImportedTextHeader(human_equipment_chest_elementalrobes_side1, png)
#include ImportedTextHeader(human_equipment_chest_elementalrobes_side2, png)
#include ImportedTextHeader(human_equipment_chest_elementalrobes_side3, png)
#include ImportedTextHeader(human_equipment_chest_shirt_idle0, png)
#include ImportedTextHeader(human_equipment_chest_shirt_idle1, png)
#include ImportedTextHeader(human_equipment_chest_shirt_side0, png)
#include ImportedTextHeader(human_equipment_chest_shirt_side1, png)
#include ImportedTextHeader(human_equipment_chest_shirt_side2, png)
#include ImportedTextHeader(human_equipment_chest_shirt_side3, png)
#include ImportedTextHeader(human_equipment_chest_warrior_idle0, png)
#include ImportedTextHeader(human_equipment_chest_warrior_idle1, png)
#include ImportedTextHeader(human_equipment_chest_warrior_side0, png)
#include ImportedTextHeader(human_equipment_chest_warrior_side1, png)
#include ImportedTextHeader(human_equipment_chest_warrior_side2, png)
#include ImportedTextHeader(human_equipment_chest_warrior_side3, png)
#include ImportedTextHeader(human_equipment_chest_warrior_up0, png)
#include ImportedTextHeader(human_equipment_chest_warrior_up1, png)
#include ImportedTextHeader(human_equipment_chest_warrior_up2, png)
#include ImportedTextHeader(human_equipment_chest_warrior_up3, png)
#include ImportedTextHeader(human_equipment_chest_warrior_down0, png)
#include ImportedTextHeader(human_equipment_chest_warrior_down1, png)
#include ImportedTextHeader(human_equipment_chest_warrior_down2, png)
#include ImportedTextHeader(human_equipment_chest_warrior_down3, png)
#include ImportedTextHeader(human_equipment_feet_dot_idle0, png)
#include ImportedTextHeader(human_equipment_feet_dot_side0, png)
#include ImportedTextHeader(human_equipment_feet_dot_side1, png)
#include ImportedTextHeader(human_equipment_feet_dot_side2, png)
#include ImportedTextHeader(human_equipment_feet_dot_side3, png)
#include ImportedTextHeader(human_equipment_feet_dot_up0, png)
#include ImportedTextHeader(human_equipment_feet_dot_up1, png)
#include ImportedTextHeader(human_equipment_feet_dot_up2, png)
#include ImportedTextHeader(human_equipment_feet_dot_up3, png)
#include ImportedTextHeader(human_equipment_feet_dot_down0, png)
#include ImportedTextHeader(human_equipment_feet_dot_down1, png)
#include ImportedTextHeader(human_equipment_feet_dot_down2, png)
#include ImportedTextHeader(human_equipment_feet_dot_down3, png)

namespace rnlib
{
	std::array<std::string_view, static_cast<int>(image_id::_count)> images
	{
		ImportedTextData(undefined, png),
		ImportedTextData(invisible, png),
		ImportedTextData(human_base_idle0, png),
		ImportedTextData(human_base_idle1, png),
		ImportedTextData(human_base_side0, png),
		ImportedTextData(human_base_side1, png),
		ImportedTextData(human_base_side2, png),
		ImportedTextData(human_base_side3, png),
		ImportedTextData(human_base_up0, png),
		ImportedTextData(human_base_up1, png),
		ImportedTextData(human_base_up2, png),
		ImportedTextData(human_base_up3, png),
		ImportedTextData(human_base_down0, png),
		ImportedTextData(human_base_down1, png),
		ImportedTextData(human_base_down2, png),
		ImportedTextData(human_base_down3, png),
		ImportedTextData(human_hair_long_idle0, png),
		ImportedTextData(human_hair_long_idle1, png),
		ImportedTextData(human_hair_long_side0, png),
		ImportedTextData(human_hair_long_side1, png),
		ImportedTextData(human_hair_long_side2, png),
		ImportedTextData(human_hair_long_side3, png),
		ImportedTextData(human_hair_long_up0, png),
		ImportedTextData(human_hair_long_up1, png),
		ImportedTextData(human_hair_long_up2, png),
		ImportedTextData(human_hair_long_up3, png),
		ImportedTextData(human_hair_long_down0, png),
		ImportedTextData(human_hair_long_down1, png),
		ImportedTextData(human_hair_long_down2, png),
		ImportedTextData(human_hair_long_down3, png),
		ImportedTextData(human_hair_lord_idle0, png),
		ImportedTextData(human_hair_lord_idle1, png),
		ImportedTextData(human_hair_lord_side0, png),
		ImportedTextData(human_hair_lord_side1, png),
		ImportedTextData(human_hair_lord_side2, png),
		ImportedTextData(human_hair_lord_side3, png),
		ImportedTextData(human_hair_lord_up0, png),
		ImportedTextData(human_hair_lord_up1, png),
		ImportedTextData(human_hair_lord_up2, png),
		ImportedTextData(human_hair_lord_up3, png),
		ImportedTextData(human_hair_lord_down0, png),
		ImportedTextData(human_hair_lord_down1, png),
		ImportedTextData(human_hair_lord_down2, png),
		ImportedTextData(human_hair_lord_down3, png),
		ImportedTextData(human_hair_prince_idle0, png),
		ImportedTextData(human_hair_prince_idle1, png),
		ImportedTextData(human_hair_prince_side0, png),
		ImportedTextData(human_hair_prince_side1, png),
		ImportedTextData(human_hair_prince_side2, png),
		ImportedTextData(human_hair_prince_side3, png),
		ImportedTextData(human_hair_prince_up0, png),
		ImportedTextData(human_hair_prince_up1, png),
		ImportedTextData(human_hair_prince_up2, png),
		ImportedTextData(human_hair_prince_up3, png),
		ImportedTextData(human_hair_prince_down0, png),
		ImportedTextData(human_hair_prince_down1, png),
		ImportedTextData(human_hair_prince_down2, png),
		ImportedTextData(human_hair_prince_down3, png),
		ImportedTextData(human_eyes_dot_idle0, png),
		ImportedTextData(human_eyes_dot_idle1, png),
		ImportedTextData(human_eyes_dot_side0, png),
		ImportedTextData(human_eyes_dot_side1, png),
		ImportedTextData(human_eyes_dot_side2, png),
		ImportedTextData(human_eyes_dot_side3, png),
		ImportedTextData(human_eyes_dot_down0, png),
		ImportedTextData(human_eyes_dot_down1, png),
		ImportedTextData(human_eyes_dot_down2, png),
		ImportedTextData(human_eyes_dot_down3, png),
		ImportedTextData(human_equipment_chest_elementalrobes_idle0, png),
		ImportedTextData(human_equipment_chest_elementalrobes_idle1, png),
		ImportedTextData(human_equipment_chest_elementalrobes_side0, png),
		ImportedTextData(human_equipment_chest_elementalrobes_side1, png),
		ImportedTextData(human_equipment_chest_elementalrobes_side2, png),
		ImportedTextData(human_equipment_chest_elementalrobes_side3, png),
		ImportedTextData(human_equipment_chest_shirt_idle0, png),
		ImportedTextData(human_equipment_chest_shirt_idle1, png),
		ImportedTextData(human_equipment_chest_shirt_side0, png),
		ImportedTextData(human_equipment_chest_shirt_side1, png),
		ImportedTextData(human_equipment_chest_shirt_side2, png),
		ImportedTextData(human_equipment_chest_shirt_side3, png),
		ImportedTextData(human_equipment_chest_warrior_idle0, png),
		ImportedTextData(human_equipment_chest_warrior_idle1, png),
		ImportedTextData(human_equipment_chest_warrior_side0, png),
		ImportedTextData(human_equipment_chest_warrior_side1, png),
		ImportedTextData(human_equipment_chest_warrior_side2, png),
		ImportedTextData(human_equipment_chest_warrior_side3, png),
		ImportedTextData(human_equipment_chest_warrior_up0, png),
		ImportedTextData(human_equipment_chest_warrior_up1, png),
		ImportedTextData(human_equipment_chest_warrior_up2, png),
		ImportedTextData(human_equipment_chest_warrior_up3, png),
		ImportedTextData(human_equipment_chest_warrior_down0, png),
		ImportedTextData(human_equipment_chest_warrior_down1, png),
		ImportedTextData(human_equipment_chest_warrior_down2, png),
		ImportedTextData(human_equipment_chest_warrior_down3, png),
		ImportedTextData(human_equipment_feet_dot_idle0, png),
		ImportedTextData(human_equipment_feet_dot_side0, png),
		ImportedTextData(human_equipment_feet_dot_side1, png),
		ImportedTextData(human_equipment_feet_dot_side2, png),
		ImportedTextData(human_equipment_feet_dot_side3, png),
		ImportedTextData(human_equipment_feet_dot_up0, png),
		ImportedTextData(human_equipment_feet_dot_up1, png),
		ImportedTextData(human_equipment_feet_dot_up2, png),
		ImportedTextData(human_equipment_feet_dot_up3, png),
		ImportedTextData(human_equipment_feet_dot_down0, png),
		ImportedTextData(human_equipment_feet_dot_down1, png),
		ImportedTextData(human_equipment_feet_dot_down2, png),
		ImportedTextData(human_equipment_feet_dot_down3, png),
	};

	tz::gl::image_resource create_image(image_id_t iid)
	{
		return load_image_data(images[iid]);
	}
}
