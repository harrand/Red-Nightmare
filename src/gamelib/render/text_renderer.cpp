#include "gamelib/render/text_renderer.hpp"
#include "tz/gl/device.hpp"
#include "tz/gl/imported_shaders.hpp"
#include <array>
#include <cstring>

#include ImportedShaderHeader(text, vertex)
#include ImportedShaderHeader(text, fragment)

namespace rnlib
{
	text_renderer::text_renderer(font f):
	rh([this, &f]()
	{
		font_data fdata = get_font(f);
		tz::gl::renderer_info rinfo;
		rinfo.shader().set_shader(tz::gl::shader_stage::vertex, ImportedShaderSource(text, vertex));
		rinfo.shader().set_shader(tz::gl::shader_stage::fragment, ImportedShaderSource(text, fragment));
		rinfo.set_options({tz::gl::renderer_option::no_depth_testing, tz::gl::renderer_option::no_clear_output});

		constexpr std::size_t max_char_count = 128;
		constexpr std::size_t max_word_count = 128;
		std::array<text_renderer::word_data, max_word_count> wdata;
		this->word_bh = rinfo.add_resource(tz::gl::buffer_resource::from_many(wdata,
		{
			.access = tz::gl::resource_access::dynamic_fixed
		}));
		std::array<char, max_char_count> cdata;
		this->string_bh = rinfo.add_resource(tz::gl::buffer_resource::from_many(cdata,
		{
			.access = tz::gl::resource_access::dynamic_fixed
		}));
		for(std::size_t i = 0; i < fdata.images.size(); i++)
		{
			tz::assert(fdata.images[i].has_value());
			rinfo.add_resource(fdata.images[i].value());
		}
		
		return tz::gl::get_device().create_renderer(rinfo);
	}())
	{

	}

	void write_char(std::uint32_t& data, std::size_t offset, char c)
	{
		std::uint32_t encoded = static_cast<std::uint32_t>(c) << (offset * 8);
		std::uint32_t mask = 0xff << (offset * 8);
		data &= ~mask;
		data |= encoded;
	}

	void text_renderer::add(std::string str, tz::vec2 pos, tz::vec2 scale)
	{
		this->words()[this->word_cursor] =
		{
			.pos = pos,
			.scale = scale,
			.char_cursor = static_cast<std::uint32_t>(this->string_cursor)
		};
		this->word_cursor++;
		for(std::size_t i = 0; i < str.size(); i++)
		{
			std::uint32_t& data = this->chars()[this->string_cursor / sizeof(std::uint32_t)];
			write_char(data, this->string_cursor % sizeof(std::uint32_t), str[i]);
			this->string_cursor++;
		}
	}

	void text_renderer::render()
	{
		// string buffer contains a large string essentially. each char requires a quad, thus 2 triangles each char.
		tz::gl::get_device().get_renderer(this->rh).render(this->string_buffer().data().size_bytes() * 2);
	}

	void text_renderer::clear()
	{

	}

	std::span<text_renderer::word_data> text_renderer::words()
	{
		return this->word_buffer().data_as<word_data>();
	}

	std::span<std::uint32_t> text_renderer::chars()
	{
		return this->string_buffer().data_as<std::uint32_t>();
	}

	const tz::gl::buffer_resource& text_renderer::word_buffer() const
	{
		return *static_cast<const tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->word_bh));
	}

	tz::gl::buffer_resource& text_renderer::word_buffer()
	{
		return *static_cast<tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->word_bh));
	}

	const tz::gl::buffer_resource& text_renderer::string_buffer() const
	{
		return *static_cast<const tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->string_bh));
	}

	tz::gl::buffer_resource& text_renderer::string_buffer()
	{
		return *static_cast<tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->string_bh));
	}
}
