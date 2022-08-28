#include "scene.hpp"
#include "tz/dbgui/dbgui.hpp"

namespace game
{
	void Scene::render()
	{
		for(std::size_t i = 0; i < this->qrenderer.elements().size(); i++)
		{
			this->qrenderer.elements()[i].texture_id = this->animations[i].get_texture();
		}
		this->qrenderer.render();
	}
	
	void Scene::dbgui()
	{
		ImGui::Text("Scene");
		ImGui::Spacing();
		if(ImGui::Button("Debug Add Player"))
		{
			this->temp_add();
		}
		if(this->size() > 0 && ImGui::Button("Pop Back"))
		{
			this->pop();
		}
		if(ImGui::Button("Clear Scene"))
		{
			this->clear();
		}
		if(ImGui::CollapsingHeader("Quad Renderer"))
		{
			this->qrenderer.dbgui();
		}
	}

	std::size_t Scene::size() const
	{
		return this->qrenderer.elements().size();
	}

	bool Scene::empty() const
	{
		return this->size() == 0;
	}

	void Scene::temp_add()
	{
		this->animations.push_back(game::play_animation(AnimationID::PlayerClassic_MoveSide));
		this->qrenderer.push();
	}
	
	void Scene::pop()
	{
		this->qrenderer.pop();
		this->animations.pop_back();
	}

	void Scene::clear()
	{
		this->qrenderer.clear();
		this->animations.clear();
	}
}
