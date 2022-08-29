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
			this->add(ActorType::PlayerClassic);
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
		if(this->size() > 0 && ImGui::CollapsingHeader("Actors"))
		{
			static int actor_id = 0;
			ImGui::SliderInt("Actor ID", &actor_id, 0, this->size() - 1);
			this->actors[actor_id].dbgui();
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
				// We tell the actor to save its motion state in the next fixed-update.
				actor.actions |= ActorAction::SceneMessage_MaintainMotion;
				{
					// But because we've done that, we want to clear out the motion state ourselves now because we're about to decide which direction its going (if we dont do this it will end up going in all 4 directions and never moving anywhere)
					constexpr std::array<ActorAction, 4> motion_actions
					{
						ActorAction::MoveLeft,
						ActorAction::MoveRight,
						ActorAction::MoveUp,
						ActorAction::MoveDown
					};
					for(auto action : motion_actions)
					{
						actor.actions.remove(action);
					}
				}

				// Choose a direction to move towards the player.
				const tz::Vec2 player_pos = this->qrenderer.elements()[player_id.value()].position;
				tz::Vec2 dist_to_player = player_pos - quad.position;
				bool move_horizontal = true, move_vertical = true;
				if(dist_to_player[0] > 0.001f)
				{

					actor.actions |= ActorAction::MoveRight;
				}
				else if(dist_to_player[0] < -0.001f)
				{
					actor.actions |= ActorAction::MoveLeft;
				}
				else
				{
					move_horizontal = false;
				}
				if(dist_to_player[1] > 0.001f)
				{
					actor.actions |= ActorAction::MoveUp;
				}
				else if(dist_to_player[1] < -0.001f)
				{
					actor.actions |= ActorAction::MoveDown;
				}
				else
				{
					move_vertical = false;
				}
				if(!move_vertical && !move_horizontal)
				{
					// You should become idle.
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
