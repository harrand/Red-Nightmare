#include "images.hpp"
#include "tz/core/imported_text.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Note: When adding a new image, add a new entry here and in game::texture_data below.
#include ImportedTextHeader(missing, png)
#include ImportedTextHeader(invisible, png)
#include ImportedTextHeader(player_classic_dead, png)
#include ImportedTextHeader(player_classic_down1, png)
#include ImportedTextHeader(player_classic_down2, png)
#include ImportedTextHeader(player_classic_down3, png)
#include ImportedTextHeader(player_classic_dying1, png)
#include ImportedTextHeader(player_classic_dying2, png)
#include ImportedTextHeader(player_classic_dying3, png)
#include ImportedTextHeader(player_classic_dying4, png)
#include ImportedTextHeader(player_classic_dying5, png)
#include ImportedTextHeader(player_classic_dying6, png)
#include ImportedTextHeader(player_classic_dying7, png)
#include ImportedTextHeader(player_classic_dying8, png)
#include ImportedTextHeader(player_classic_idle1, png)
#include ImportedTextHeader(player_classic_idle2, png)
#include ImportedTextHeader(player_classic_side1, png)
#include ImportedTextHeader(player_classic_side2, png)
#include ImportedTextHeader(player_classic_side3, png)
#include ImportedTextHeader(player_classic_special, png)
#include ImportedTextHeader(player_classic_up1, png)
#include ImportedTextHeader(player_classic_up2, png)
#include ImportedTextHeader(player_classic_up3, png)

#include ImportedTextHeader(player_classic_default_fireball, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_0, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_1, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_2, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_3, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_4, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_5, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_6, png)

#include ImportedTextHeader(nightmare_dead, png)
#include ImportedTextHeader(nightmare_down1, png)
#include ImportedTextHeader(nightmare_down2, png)
#include ImportedTextHeader(nightmare_down3, png)
#include ImportedTextHeader(nightmare_dying1, png)
#include ImportedTextHeader(nightmare_dying2, png)
#include ImportedTextHeader(nightmare_dying3, png)
#include ImportedTextHeader(nightmare_dying4, png)
#include ImportedTextHeader(nightmare_dying5, png)
#include ImportedTextHeader(nightmare_dying6, png)
#include ImportedTextHeader(nightmare_dying7, png)
#include ImportedTextHeader(nightmare_dying8, png)
#include ImportedTextHeader(nightmare_dying9, png)
#include ImportedTextHeader(nightmare_dying10, png)
#include ImportedTextHeader(nightmare_idle1, png)
#include ImportedTextHeader(nightmare_idle2, png)
#include ImportedTextHeader(nightmare_side1, png)
#include ImportedTextHeader(nightmare_side2, png)
#include ImportedTextHeader(nightmare_side3, png)
#include ImportedTextHeader(nightmare_special, png)
#include ImportedTextHeader(nightmare_up1, png)
#include ImportedTextHeader(nightmare_up2, png)
#include ImportedTextHeader(nightmare_up3, png)

namespace game
{
	std::array<std::string_view, static_cast<int>(TextureID::Count)> texture_data
	{
		ImportedTextData(missing, png),
		ImportedTextData(invisible, png),
		ImportedTextData(player_classic_dead, png),
		ImportedTextData(player_classic_down1, png),
		ImportedTextData(player_classic_down2, png),
		ImportedTextData(player_classic_down3, png),
		ImportedTextData(player_classic_dying1, png),
		ImportedTextData(player_classic_dying2, png),
		ImportedTextData(player_classic_dying3, png),
		ImportedTextData(player_classic_dying4, png),
		ImportedTextData(player_classic_dying5, png),
		ImportedTextData(player_classic_dying6, png),
		ImportedTextData(player_classic_dying7, png),
		ImportedTextData(player_classic_dying8, png),
		ImportedTextData(player_classic_idle1, png),
		ImportedTextData(player_classic_idle2, png),
		ImportedTextData(player_classic_side1, png),
		ImportedTextData(player_classic_side2, png),
		ImportedTextData(player_classic_side3, png),
		ImportedTextData(player_classic_special, png),
		ImportedTextData(player_classic_up1, png),
		ImportedTextData(player_classic_up2, png),
		ImportedTextData(player_classic_up3, png),

		ImportedTextData(player_classic_default_fireball, png),
		ImportedTextData(player_classic_default_fireball_particles_0, png),
		ImportedTextData(player_classic_default_fireball_particles_1, png),
		ImportedTextData(player_classic_default_fireball_particles_2, png),
		ImportedTextData(player_classic_default_fireball_particles_3, png),
		ImportedTextData(player_classic_default_fireball_particles_4, png),
		ImportedTextData(player_classic_default_fireball_particles_5, png),
		ImportedTextData(player_classic_default_fireball_particles_6, png),

		ImportedTextData(nightmare_dead, png),
		ImportedTextData(nightmare_down1, png),
		ImportedTextData(nightmare_down2, png),
		ImportedTextData(nightmare_down3, png),
		ImportedTextData(nightmare_dying1, png),
		ImportedTextData(nightmare_dying2, png),
		ImportedTextData(nightmare_dying3, png),
		ImportedTextData(nightmare_dying4, png),
		ImportedTextData(nightmare_dying5, png),
		ImportedTextData(nightmare_dying6, png),
		ImportedTextData(nightmare_dying7, png),
		ImportedTextData(nightmare_dying8, png),
		ImportedTextData(nightmare_dying9, png),
		ImportedTextData(nightmare_dying10, png),
		ImportedTextData(nightmare_idle1, png),
		ImportedTextData(nightmare_idle2, png),
		ImportedTextData(nightmare_side1, png),
		ImportedTextData(nightmare_side2, png),
		ImportedTextData(nightmare_side3, png),
		ImportedTextData(nightmare_special, png),
		ImportedTextData(nightmare_up1, png),
		ImportedTextData(nightmare_up2, png),
		ImportedTextData(nightmare_up3, png)
	};

	tz::gl::ImageResource load_image(TextureID texid)
	{
		std::string_view img_file_data = texture_data[static_cast<int>(texid)];
		int w, h, channels;
		stbi_uc* imgdata = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(img_file_data.data()), img_file_data.size(), &w, &h, &channels, 4);
		std::span<stbi_uc> imgdata_span{imgdata, static_cast<std::size_t>(w * h * 4)};
		tz::gl::ImageResource ret = tz::gl::ImageResource::from_memory(tz::gl::ImageFormat::RGBA32, static_cast<tz::Vec2ui>(tz::Vec2i{w, h}), imgdata_span, tz::gl::ResourceAccess::StaticFixed, {});
		stbi_image_free(imgdata);
		return ret;
	}
}
