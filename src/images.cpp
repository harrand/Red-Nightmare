#include "images.hpp"
#include "tz/core/imported_text.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Note: When adding a new image, add a new entry here and in game::texture_data below.
#include ImportedTextHeader(idle1, png)
#include ImportedTextHeader(idle2, png)

namespace game
{
	std::array<std::string_view, static_cast<int>(TextureID::Count)> texture_data
	{
		ImportedTextData(idle1, png),
		ImportedTextData(idle2, png)
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
