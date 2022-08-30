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
		ImGui::Text("Current Scene");
		ImGui::Spacing();
		if(ImGui::CollapsingHeader("Mass Control"))
		{
			if(ImGui::Button("Kill Everyone"))
			{
				for(auto& actor : this->actors)
				{
					actor.current_health = 0;
				}
			}
			if(ImGui::Button("Resurrect Everyone"))
			{
				for(auto& actor : this->actors)
				{
					actor.current_health = actor.max_health;
				}
			}
			if(ImGui::Button("Respawn Everyone"))
			{
				for(auto& actor : this->actors)
				{
					actor.respawn();
				}
			}
			if(ImGui::Button("Clear Scene"))
			{
				this->clear();
			}
		}
		if(ImGui::Button("Debug Add Player"))
		{
			this->add(ActorType::PlayerClassic);
		}
		if(ImGui::Button("Debug Add Evil Player"))
		{
			this->add(ActorType::PlayerClassic_TestEvil);
		}
		if(ImGui::Button("Debug Add Nightmare Boss"))
		{
			this->add(ActorType::Nightmare);
		}
		if(this->size() > 0 && ImGui::Button("Pop Back"))
		{
			this->pop();
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
		// If actor wants to teleport to a random location, do it now.
		if(actor.actions.contains(ActorAction::RandomTeleport))
		{
			std::uniform_real_distribution<float> dist{-1.0f, 1.0f};
			quad.position[0] = dist(this->rng);
			quad.position[1] = dist(this->rng);
		}
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
		std::optional<tz::Vec2> chase_target = std::nullopt;
		if(actor.actions.contains(ActorAction::ChasePlayer))
		{
			auto player_id = this->find_first_player();
			if(player_id.has_value())
			{
				chase_target = this->qrenderer.elements()[player_id.value()].position;
			}
		}
		if(actor.actions.contains(ActorAction::FollowMouse))
		{
			tz::Vec2 mouse_pos = static_cast<tz::Vec2>(tz::window().get_mouse_position_state().get_mouse_position());
			mouse_pos[0] /= tz::window().get_width();
			mouse_pos[1] /= tz::window().get_height();
			mouse_pos *= 2.0f;
			mouse_pos -= tz::Vec2{1.0f, 1.0f};
			mouse_pos[1] = -mouse_pos[1];
			chase_target = mouse_pos;
		}
		if(chase_target.has_value())
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
			const tz::Vec2 target_pos = chase_target.value();
			tz::Vec2 dist_to_target = target_pos - quad.position;
			bool move_horizontal = true, move_vertical = true;
			constexpr float touch_distance = 0.14f;
			if(dist_to_target[0] > touch_distance)
			{

				actor.actions |= ActorAction::MoveRight;
			}
			else if(dist_to_target[0] < -touch_distance)
			{
				actor.actions |= ActorAction::MoveLeft;
			}
			else
			{
				move_horizontal = false;
			}
			if(dist_to_target[1] > touch_distance)
			{
				actor.actions |= ActorAction::MoveUp;
			}
			else if(dist_to_target[1] < -touch_distance)
			{
				actor.actions |= ActorAction::MoveDown;
			}
			else
			{
				move_vertical = false;
			}
			if(!move_vertical && !move_horizontal)
			{
				// Something chasing the player has reached the player.
				if(actor.flags.contains(ActorFlag::HostileGhost) && !actor.actions.contains(ActorAction::FollowMouse) && !actor.dead())
				{
					auto target_actor_id = this->find_first_player();
					if(target_actor_id.has_value())
					{
						actor.damage(this->actors[target_actor_id.value()]);
					}
					// If hostile ghost touches a player, damage it.
					//player_actor.current_health -= actor.base_damage;
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
			if(this->actors[i].flags.contains(ActorFlag::Player) && !this->actors[i].dead())
			{
				return {i};
			}
		}
		return std::nullopt;
	}
}
