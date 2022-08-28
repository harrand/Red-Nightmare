#include "images.hpp"
#include "tz/core/imported_text.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Note: When adding a new image, add a new entry here and in game::texture_data below.
#include ImportedTextHeader(dead, png)
#include ImportedTextHeader(down1, png)
#include ImportedTextHeader(down2, png)
#include ImportedTextHeader(down3, png)
#include ImportedTextHeader(dying1, png)
#include ImportedTextHeader(dying2, png)
#include ImportedTextHeader(dying3, png)
#include ImportedTextHeader(dying4, png)
#include ImportedTextHeader(dying5, png)
#include ImportedTextHeader(dying6, png)
#include ImportedTextHeader(dying7, png)
#include ImportedTextHeader(dying8, png)
#include ImportedTextHeader(idle1, png)
#include ImportedTextHeader(idle2, png)
#include ImportedTextHeader(side1, png)
#include ImportedTextHeader(side2, png)
#include ImportedTextHeader(side3, png)
#include ImportedTextHeader(special, png)
#include ImportedTextHeader(up1, png)
#include ImportedTextHeader(up2, png)
#include ImportedTextHeader(up3, png)

namespace game
{
	std::array<std::string_view, static_cast<int>(TextureID::Count)> texture_data
	{
		ImportedTextData(dead, png),
		ImportedTextData(down1, png),
		ImportedTextData(down2, png),
		ImportedTextData(down3, png),
		ImportedTextData(dying1, png),
		ImportedTextData(dying2, png),
		ImportedTextData(dying3, png),
		ImportedTextData(dying4, png),
		ImportedTextData(dying5, png),
		ImportedTextData(dying6, png),
		ImportedTextData(dying7, png),
		ImportedTextData(dying8, png),
		ImportedTextData(idle1, png),
		ImportedTextData(idle2, png),
		ImportedTextData(side1, png),
		ImportedTextData(side2, png),
		ImportedTextData(side3, png),
		ImportedTextData(special, png),
		ImportedTextData(up1, png),
		ImportedTextData(up2, png),
		ImportedTextData(up3, png)
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
