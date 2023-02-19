#include "gamelib/render/font.hpp"
#include "gamelib/render/image.hpp"
#include "tz/core/debug.hpp"
#include "tz/core/imported_text.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <span>

#include ImportedTextHeader(invisible, png)
#include ImportedTextHeader(LucidaSansRegular, ttf)

namespace rnlib
{
	FT_Library ftlib;

	void font_system_initialise()
	{
		int err = FT_Init_FreeType(&ftlib);
		tz::assert(err == 0, "failed to initialise font system. `FT_Init_FreeType` returned erroneous code %d", err);	
	}

	void font_system_terminate()
	{
		[[maybe_unused]] int err = FT_Done_FreeType(ftlib);
		tz::assert(err == 0, "failed to terminate font system. `FT_Done_FreeType` returned erroneous code %d", err);	
		ftlib = {};
	}

	font_data get_font(font f)
	{
		std::string_view fontdata;
		switch(f)
		{
			case font::lucida_sans_regular:
				fontdata = ImportedTextData(LucidaSansRegular, ttf);
			break;
		}
		FT_Face ftfont;
		int err = FT_New_Memory_Face(ftlib, reinterpret_cast<const FT_Byte*>(fontdata.data()), fontdata.size(), 0, &ftfont);
		tz::assert(err == 0, "failed to load font %d", static_cast<int>(f));

		FT_Set_Pixel_Sizes(ftfont, 0, 64);
		font_data ret;
		ret.glyphs[0] = font_glyph
		{
			.image = rnlib::load_image_data(ImportedTextData(invisible, png))
		};
		// lowercase chars.
		auto load_char = [ftfont](char c)->font_glyph
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
			tz::vec2 bounding_box_min = tz::vec2::zero();
			// these values are in 26.6 fixed point format.
			// to convert we just divide by 65535;
			bounding_box_min[0] = static_cast<float>(ftfont->glyph->bitmap_left) / width;
			bounding_box_min[1] = 1.0f - (static_cast<float>(ftfont->glyph->bitmap_top) / height);
			auto dims = static_cast<tz::vec2>(tz::vector<long int, 2>{ftfont->glyph->metrics.width / 64, ftfont->glyph->metrics.height / 64});
			tz::vec2 bounding_box_max = bounding_box_min + dims;
			bounding_box_max[0] /= width;
			bounding_box_max[1] /= height;
			return 
			{
				.image = tz::gl::image_resource::from_memory(imgdata_rgba8,
				{
					.format = tz::gl::image_format::RGBA32,
					.dimensions = {width, height}
				}),
				.min = bounding_box_min,
				.max = bounding_box_max,
				.to_next = static_cast<float>(ftfont->glyph->metrics.horiAdvance >> 6)
			};
		};
		for(std::size_t i = 0; i < 26; i++)
		{
			ret.glyphs[1 + i] = load_char('a' + i);
		}

		for(std::size_t i = 0; i < 26; i++)
		{
			ret.glyphs[1 + 26 + i] = load_char('A' + i);
		}

		for(std::size_t i = 0; i < 10; i++)
		{
			ret.glyphs[1 + 26 + 26 + i] = load_char('0' + i);
		}
		FT_Done_Face(ftfont);
		return ret;
	}
}
