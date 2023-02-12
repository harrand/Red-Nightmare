#ifndef RNLIB_RENDER_TEXT_RENDERER_HPP
#define RNLIB_RENDER_TEXT_RENDERER_HPP
#include "gamelib/render/font.hpp"
#include "tz/gl/renderer.hpp"
#include "tz/gl/resource.hpp"

namespace rnlib
{
	class text_renderer
	{
	public:
		text_renderer(font f);

		void add(std::string str, tz::vec2 pos, tz::vec2 scale);
		void render();
		void clear();
	private:
		struct word_data
		{
			tz::vec2 pos = tz::vec2::zero();
			tz::vec2 scale = tz::vec2::zero();
			std::uint32_t char_cursor = std::uint32_t(-1);
			std::uint32_t length = 0;
		};
		std::span<word_data> words();
		std::span<std::uint32_t> chars();
		const tz::gl::buffer_resource& word_buffer() const;
		tz::gl::buffer_resource& word_buffer();
		const tz::gl::buffer_resource& string_buffer() const;
		tz::gl::buffer_resource& string_buffer();

		tz::gl::resource_handle word_bh = tz::nullhand;
		tz::gl::resource_handle string_bh = tz::nullhand;
		tz::gl::resource_handle glyph_bh = tz::nullhand;
		tz::gl::renderer_handle rh;
		std::size_t word_cursor = 0;
		std::size_t string_cursor = 0;
	};
}

#endif // RNLIB_RENDER_TEXT_RENDERER_HPP
