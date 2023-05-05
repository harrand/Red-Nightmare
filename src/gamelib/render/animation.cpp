#include "gamelib/render/animation.hpp"
#include "tz/dbgui/dbgui.hpp"
#include <algorithm>

namespace rnlib
{
	animation::animation(animation_data data):
	data(data){}

	std::uint32_t animation::get_image() const
	{
		if(this->data.frame_textures.empty())
		{
			// assume 1 is a magic number that represents an invisible image. i know its awful.
			return 1;
		}
		const auto frame_count = static_cast<unsigned int>(this->data.frame_textures.size());
		const float frame_period = 1.0f / this->data.fps;

		float true_elapsed = this->elapsed * this->time_multiplier;
		unsigned int frame_number = true_elapsed / frame_period;
		if(!this->data.loop)
		{
			frame_number = std::clamp(frame_number, 0u, frame_count - 1);
		}
		else
		{
			frame_number = frame_number % frame_count;
		}
		return this->data.frame_textures[frame_number];
	}

	void animation::update(float dt)
	{
		// assume dt is in millis.
		this->elapsed += (dt / 1000.0f);
	}

	void animation::dbgui()
	{
		ImGui::InputFloat("Time Multiplier", &this->time_multiplier);
		if(this->data.frame_textures.size())
		{
			ImGui::Text("Frames (%zu): {", this->data.frame_textures.size());
			ImGui::SameLine();
			const char* selected;
			for(std::uint32_t f : this->data.frame_textures)
			{
				if(f == this->get_image())
				{
					selected = "*";
				}
				else
				{
					selected = "";
				}
				ImGui::Text("%s%zu%s", selected, static_cast<std::size_t>(f), selected);
				ImGui::SameLine();
			}
			ImGui::Text("}");
		}
		if(ImGui::Button("Restart"))
		{
			this->elapsed = 0.0f;
		}
	}

	std::size_t animation::size() const
	{
		return this->data.frame_textures.size();
	}
}
