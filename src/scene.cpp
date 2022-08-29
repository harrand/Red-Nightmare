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
		if(ImGui::Button("Debug Add Evil Player"))
		{
			this->add(ActorType::PlayerClassic_TestEvil);
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

		const float sp = actor.base_movement;
		// Handle actions.
		if(actor.actions.contains(ActorAction::ChasePlayer))
		{
			auto player_id = this->find_first_player();
			if(player_id.has_value())
			{
				// Choose a direction to move towards the player.
				tz::Vec2 dist_to_player = this->qrenderer.elements()[player_id.value()].position - quad.position;
				if(dist_to_player[0] > 0.001f)
				{

					actor.actions |= ActorAction::MoveRight;
				}
				else if(dist_to_player[0] < -0.001f)
				{
					actor.actions |= ActorAction::MoveLeft;
				}
				if(dist_to_player[1] > 0.001f)
				{
					actor.actions |= ActorAction::MoveUp;
				}
				else if(dist_to_player[1] < -0.001f)
				{
					actor.actions |= ActorAction::MoveDown;
				}
			}
		}
		if(actor.actions.contains(ActorAction::MoveLeft))
		{
			// Actor wants to move left.
			quad.position[0] -= sp;
		}
		if(actor.actions.contains(ActorAction::MoveRight))
		{
			// Actor wants to move right.
			quad.position[0] += sp;
		}
		if(actor.actions.contains(ActorAction::MoveUp))
		{
			quad.position[1] += sp;
		}
		if(actor.actions.contains(ActorAction::MoveDown))
		{
			quad.position[1] -= sp;
		}
	}

	std::optional<std::size_t> Scene::find_first_player() const
	{
		for(std::size_t i = 0; i < this->size(); i++)
		{
			if(this->actors[i].flags.contains(ActorFlag::Player))
			{
				return {i};
			}
		}
		return std::nullopt;
	}
}
