#include "scene.hpp"
#include "tz/dbgui/dbgui.hpp"

namespace game
{
	void Scene::render()
	{
		tz_assert(this->actors.size() == this->qrenderer.elements().size(), "Scene actor list and QuadRenderer size no longer match. Logic Error");
		for(std::size_t i = 0; i < this->qrenderer.elements().size(); i++)
		{
			this->qrenderer.elements()[i].texture_id = this->actors[i].animation.get_texture();
		}
		this->qrenderer.render();
	}

	void Scene::update()
	{
		for(std::size_t i = 0; i < this->size(); i++)
		{
			Actor& actor = this->actors[i];

			actor.update();
			this->actor_post_update(i);
		}
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
		this->add(ActorType::PlayerClassic);
	}

	void Scene::add(ActorType type)
	{
		this->actors.push_back(game::create_actor(type));
		this->qrenderer.push();
	}
	
	void Scene::pop()
	{
		this->qrenderer.pop();
		this->actors.pop_back();
	}

	void Scene::clear()
	{
		this->qrenderer.clear();
		this->actors.clear();
	}

	void Scene::actor_post_update(std::size_t id)
	{
		Actor& actor = this->actors[id];
		QuadRenderer::ElementData& quad = this->qrenderer.elements()[id];
		// If actor wants to flip horizontally, do that now.
		if(actor.actions.contains(ActorAction::HorizontalFlip))
		{
			quad.scale[0] = -std::abs(quad.scale[0]);
		}
		else
		{
			quad.scale[0] = std::abs(quad.scale[0]);
		}
	}
}
