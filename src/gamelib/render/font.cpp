#include "gamelib/render/font.hpp"
#include "tz/core/debug.hpp"
#include "tz/core/imported_text.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <span>

namespace rnlib
{
	FT_Library ftlib;

	struct font_system
	{
		std::vector<FT_Face> font_faces;
	} fsys;

	FT_Face load_font(font f);

	void font_system_initialise()
	{
		int err = FT_Init_FreeType(&ftlib);
		tz::assert(err == 0, "failed to initialise font system. `FT_Init_FreeType` returned erroneous code %d", err);	
		constexpr int font_count = static_cast<int>(font::_count);
		fsys.font_faces.resize(font_count);
		for(int i = 0; i < font_count; i++)
		{
			fsys.font_faces[i] = load_font(static_cast<font>(i));
		}
	}

	void font_system_terminate()
	{
		[[maybe_unused]] int err = FT_Done_FreeType(ftlib);
		for(FT_Face font_face : fsys.font_faces)
		{
			FT_Done_Face(font_face);
		}
		fsys = {};
		tz::assert(err == 0, "failed to terminate font system. `FT_Done_FreeType` returned erroneous code %d", err);	
		ftlib = {};
	}

	font_data get_font(font f)
	{
		FT_Face ftfont = load_font(f);
		FT_Set_Pixel_Sizes(ftfont, 0, 64);
		font_data ret;
		// lowercase chars.
		auto load_char = [ftfont](char c)->tz::gl::image_resource
		{
			FT_Load_Char(ftfont, c, FT_LOAD_RENDER);	
			// bitmap contains 8-bit grayscale image. we convert to RGBA8.
			unsigned int width = ftfont->glyph->bitmap.width;
			unsigned int height = ftfont->glyph->bitmap.rows;
			tz::assert(width != 0 && height != 0);
			unsigned char* imgdata_r8 = ftfont->glyph->bitmap.buffer;
			std::vector<std::byte> imgdata_rgba8;
			imgdata_rgba8.reserve(width * height * 4);
			for(std::size_t i = 0; i < (width * height); i++)
			{
				auto pix = static_cast<std::byte>(imgdata_r8[i]);
				imgdata_rgba8.push_back(pix);
				imgdata_rgba8.push_back(pix);
				imgdata_rgba8.push_back(pix);
				imgdata_rgba8.push_back(pix);
			}
			return tz::gl::image_resource::from_memory(imgdata_rgba8,
			{
				.format = tz::gl::image_format::RGBA32,
				.dimensions = {width, height},
			});
		};
		for(std::size_t i = 0; i < 26; i++)
		{
			ret.images[i] = load_char('a' + i);
		}

		for(std::size_t i = 0; i < 26; i++)
		{
			ret.images[26 + i] = load_char('A' + i);
		}

		for(std::size_t i = 0; i < 10; i++)
		{
			ret.images[26 + 26 + i] = load_char('0' + i);
		}
		return ret;
	}
}

#include ImportedTextHeader(LucidaSansRegular, ttf)
namespace rnlib
{
	FT_Face load_font(font f)
	{
		std::string_view fontdata;
		switch(f)
		{
			case font::lucida_sans_regular:
				fontdata = ImportedTextData(LucidaSansRegular, ttf);
			break;
		}
		FT_Face face;
		int err = FT_New_Memory_Face(ftlib, reinterpret_cast<const FT_Byte*>(fontdata.data()), fontdata.size(), 0, &face);
		tz::assert(err == 0, "failed to load font %d", static_cast<int>(f));
		return face;
	}
}
