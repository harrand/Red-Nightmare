#include "gamelib/render/text_renderer.hpp"
#include "tz/core/profile.hpp"
#include "tz/gl/device.hpp"
#include "tz/gl/draw.hpp"
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
		TZ_PROFZONE("text_renderer - initialise", 0xff0077ee);
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
			.access = tz::gl::resource_access::dynamic_variable
		}));
		std::array<char, max_char_count> cdata;
		this->string_bh = rinfo.add_resource(tz::gl::buffer_resource::from_many(cdata,
		{
			.access = tz::gl::resource_access::dynamic_variable
		}));
		this->indirect_bh = rinfo.add_resource(tz::gl::buffer_resource::from_one(tz::gl::draw_indirect_command
		{
			.count = 0u,
			.first = 0
		},
		{
			.access = tz::gl::resource_access::dynamic_fixed,
			.flags = {tz::gl::resource_flag::draw_indirect_buffer}
		}));
		rinfo.state().graphics.draw_buffer = this->indirect_bh;
		struct glyph_data
	   	{
			tz::vec2 min = {};
			tz::vec2 max = {};
			float to_next = 0.0f;
			float pad0 = 0.0f;
		};
		std::array<glyph_data, fdata.glyphs.size()> glyphs;
		std::transform(fdata.glyphs.begin(), fdata.glyphs.end(), glyphs.begin(),
		[](const font_glyph& glyph)
		{
			return glyph_data{.min = glyph.min, .max = glyph.max, .to_next = glyph.to_next};
		});
		this->glyph_bh = rinfo.add_resource(tz::gl::buffer_resource::from_many(glyphs));
		this->data_bh = rinfo.add_resource(tz::gl::buffer_resource::from_one(render_data{},
		{
			.access = tz::gl::resource_access::dynamic_fixed
		}));
		for(std::size_t i = 0; i < fdata.glyphs.size(); i++)
		{
			rinfo.add_resource(fdata.glyphs[i].image);
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
			.char_cursor = static_cast<std::uint32_t>(this->string_cursor),
			.length = static_cast<std::uint32_t>(str.size())
		};
		this->word_cursor++;
		if(this->word_cursor >= this->words().size())
		{
			// ran out of words space. need to resize buffer.
			tz::gl::get_device().get_renderer(this->rh).edit
			(
				tz::gl::RendererEditBuilder{}
				.buffer_resize({.buffer_handle = this->word_bh, .size = static_cast<std::uint32_t>(this->word_buffer().data().size_bytes() * 2)})
				.build()
			);
			tz::report("resized word buffer to %zu", this->word_buffer().data().size_bytes());
		}
		if(this->string_cursor + str.size() >= this->chars().size_bytes())
		{
			// ran out of chars space. need to resize buffer.
			tz::gl::get_device().get_renderer(this->rh).edit
			(
				tz::gl::RendererEditBuilder{}
				.buffer_resize({.buffer_handle = this->string_bh, .size = static_cast<std::uint32_t>(this->string_buffer().data().size_bytes() * 2)})
				.build()
			);
			tz::report("resized string buffer to %zu", this->string_buffer().data().size_bytes());
		}
		for(std::size_t i = 0; i < str.size(); i++)
		{
			std::uint32_t& data = this->chars()[this->string_cursor / sizeof(std::uint32_t)];
			write_char(data, this->string_cursor % sizeof(std::uint32_t), str[i]);
			this->string_cursor++;
		}
	}

	void text_renderer::render()
	{
		TZ_PROFZONE("text_renderer - render", 0xffee0077);
		// string buffer contains a large string essentially. each char requires a quad, thus 2 triangles each char.
		this->indirect_buffer().data_as<tz::gl::draw_indirect_command>().front().count = this->string_buffer().data().size_bytes() * 2 * 3;
	}

	void text_renderer::clear()
	{
		auto wdata = this->words();
		std::fill(wdata.begin(), wdata.end(), word_data{});
		auto string_data = this->chars();
		std::fill(string_data.begin(), string_data.end(), std::uint32_t{0});
		this->word_cursor = 0;
		this->string_cursor = 0;
	}

	void text_renderer::set_render_data(render_data data)
	{
		this->data_buffer().data_as<render_data>().front() = data;
	}

	void text_renderer::dbgui()
	{
		if(ImGui::BeginTabBar("Text Renderer"))
		{
			if(ImGui::BeginTabItem("Text Editor"))
			{
				if(ImGui::CollapsingHeader("Create..."))
				{
					static std::array<char, 256> data = {0};
					static tz::vec2 pos = tz::vec2::zero();
					static tz::vec2 scale = tz::vec2::filled(0.05f);
					ImGui::InputText("Text", data.data(), data.size());
					ImGui::InputFloat2("Position", pos.data().data());
					ImGui::InputFloat2("Scale", scale.data().data());
					if(ImGui::Button("Submit"))
					{
						this->add(std::string{data.data()}, pos, scale);
					}
				}
				if(ImGui::Button("Clear"))
				{
					this->clear();
				}
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Word Data"))
			{
				if(this->word_cursor == 0)
				{
					ImGui::Text("no data");
				}
				else
				{
					static int word_id = 0;
					ImGui::SliderInt("Word ID", &word_id, 0, this->word_cursor - 1);
					ImGui::Indent();
					auto& word = this->words()[word_id];
					ImGui::InputFloat2("Pos", word.pos.data().data());	
					ImGui::InputFloat2("Scale", word.scale.data().data());
					ImGui::SliderInt("Char Cursor", reinterpret_cast<int*>(&word.char_cursor), 0, this->string_cursor - 1, "%zu");
					ImGui::SliderInt("Length", reinterpret_cast<int*>(&word.length), 0, (this->string_cursor - word.char_cursor), "%zu");	
					ImGui::Unindent();
				}
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("String Data"))
			{
				auto bytes = std::as_writable_bytes(this->chars());
				ImGui::InputText("Data", reinterpret_cast<char*>(bytes.data()), bytes.size_bytes());
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}

	tz::gl::renderer_handle text_renderer::get() const
	{
		return this->rh;
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

	const tz::gl::buffer_resource& text_renderer::data_buffer() const
	{
		return *static_cast<const tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->data_bh));
	}

	tz::gl::buffer_resource& text_renderer::data_buffer()
	{
		return *static_cast<tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->data_bh));
	}

	tz::gl::buffer_resource& text_renderer::indirect_buffer()
	{
		return *static_cast<tz::gl::buffer_resource*>(tz::gl::get_device().get_renderer(this->rh).get_resource(this->indirect_bh));
	}
}
