#include "scene.hpp"
#include "util.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "tz/core/assert.hpp"

#include "tz/core/report.hpp"

namespace game
{
	constexpr float touch_distance = 0.14f;

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
		if(ImGui::Button("Debug Add Evil Player Spawner"))
		{
			this->add(ActorType::EvilPlayer_TestSpawner);
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
		if(!this->is_in_bounds(id))
		{
			if(actor.flags.contains(ActorFlag::DieIfOOB))
			{
				actor.current_health = 0;
			}
			if(actor.flags.contains(ActorFlag::RespawnIfOOB))
			{
				actor.respawn();
			}
		}
		if(actor.flags.contains(ActorFlag::Haunted) && actor.dead())
		{
			// Note: Right now this could spam forever.
			this->add(ActorType::PlayerClassic_TestEvil);
			// Teleport it to our location.
			this->qrenderer.elements().back().position = quad.position;
		}
		// If actor wants to teleport to a random location, do it now.
		if(actor.actions.contains(ActorAction::RandomTeleport))
		{
			std::uniform_real_distribution<float> dist{-1.0f, 1.0f};
			quad.position[0] = dist(this->rng);
			quad.position[1] = dist(this->rng);
		}

		if(actor.flags.contains(ActorFlag::InvisibleWhileDead))
		{
			if(actor.dead())
			{
				quad.scale *= 0.0f;
			}
			else
			{
				// default scale. TODO: dont hardcode
				quad.scale = {0.2f, 0.2f};
			}
		}

		// Handle actions.
		std::optional<tz::Vec2> chase_target = std::nullopt;
		// Actor will chase whatever the player's position is.
		if(actor.actions.contains(ActorAction::ChasePlayer))
		{
			auto player_id = this->find_first_player();
			if(player_id.has_value())
			{
				chase_target = this->qrenderer.elements()[player_id.value()].position;
			}
		}

		// Entity Actions

		// Recursive Entity Actions (Can add other actions).
		if(actor.entity.has<ActionID::GotoMouse>())
		{
			actor.entity.add<ActionID::GotoTarget>
			({
				.target_position = util::get_mouse_world_location()
			});
			actor.entity.get<ActionID::GotoMouse>()->set_is_complete(true);
		}
		if(actor.entity.has<ActionID::LaunchToMouse>())
		{
			auto action = actor.entity.get<ActionID::LaunchToMouse>();
			tz::Vec2 mouse_pos = util::get_mouse_world_location();
			tz::Vec2 to_mouse = mouse_pos - quad.position;
			actor.entity.add<ActionID::Launch>
			({
				.direction = to_mouse,
				.speed_multiplier = action->data().speed_multiplier
			});
		}
		if(actor.entity.has<ActionID::TeleportToPlayer>())
		{
			auto maybe_player_id = this->find_first_player();
			if(maybe_player_id.has_value())
			{
				actor.entity.add<ActionID::Teleport>
				({
					.position = this->qrenderer.elements()[maybe_player_id.value()].position
				});
			}
			actor.entity.get<ActionID::TeleportToPlayer>()->set_is_complete(true);
		}
		// Concrete Entity Actions
		if(actor.entity.has<ActionID::GotoTarget>())
		{
			chase_target = actor.entity.get<ActionID::GotoTarget>()->data().target_position;
		}
		if(actor.entity.has<ActionID::Launch>())
		{
			auto action = actor.entity.get<ActionID::Launch>();
			const float speed = actor.base_movement * action->data().speed_multiplier;
			quad.position += action->data().direction.normalised() * speed;
		}
		if(actor.entity.has<ActionID::Teleport>())
		{
			auto action = actor.entity.get<ActionID::Teleport>();
			quad.position = action->data().position;
			action->set_is_complete(true);
		}
		if(actor.entity.has<ActionID::HorizontalFlip>())
		{
			auto action = actor.entity.get<ActionID::HorizontalFlip>();
			quad.scale[0] = -std::abs(quad.scale[0]);
			action->set_is_complete(true);
		}
		else
		{
			quad.scale[0] = std::abs(quad.scale[0]);
		}
		if(actor.entity.has<ActionID::VerticalFlip>())
		{
			auto action = actor.entity.get<ActionID::VerticalFlip>();
			quad.scale[1] = -std::abs(quad.scale[1]);
			action->set_is_complete(true);
		}
		else
		{
			quad.scale[1] = std::abs(quad.scale[1]);
		}

		// Actor will chase the mouse cursor location relative to the game window.
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
		// It's chasing something, but we don't care about what it's chasing.
		if(chase_target.has_value())
		{
			// We tell the actor to save its motion state in the next fixed-update, so it can decide on which animation to use based upon the direction we're making it move.
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

			// Get the displacement between the actor and its chase target.
			const tz::Vec2 target_pos = chase_target.value();
			tz::Vec2 dist_to_target = target_pos - quad.position;
			// Find out which direction we need to go, or if we're already at the target.
			bool move_horizontal = true, move_vertical = true;
			if(dist_to_target[0] > touch_distance)
			{
				// We need to move right.
				actor.actions |= ActorAction::MoveRight;
			}
			else if(dist_to_target[0] < -touch_distance)
			{
				// We need to move left.
				actor.actions |= ActorAction::MoveLeft;
			}
			else
			{
				// We don't need to move horizontally, we're x-aligned with our chase target.
				move_horizontal = false;
			}
			if(dist_to_target[1] > touch_distance)
			{
				// We need to move upwards.
				actor.actions |= ActorAction::MoveUp;
			}
			else if(dist_to_target[1] < -touch_distance)
			{
				// We need to move downwards.
				actor.actions |= ActorAction::MoveDown;
			}
			else
			{
				// We don't need to move vertically, we're y-aligned with our chase target.
				move_vertical = false;
			}
			if(!move_vertical && !move_horizontal)
			{
				// Something chasing a target has reached it.

				if(actor.flags.contains(ActorFlag::DieAtRest))
				{
					actor.current_health = 0;
				}
				if(actor.actions.contains(ActorAction::FollowMouse))
				{
					// Something following the mouse cursor has reached it.
					if(actor.flags.contains(ActorFlag::ChaseMouse))
					{
						actor.flags.remove(ActorFlag::ChaseMouse);
					}
				}
				else if(actor.faction == Faction::PlayerEnemy && !actor.dead())
				{
					// The living actor is an enemy, and should have finished chasing the player. We need to find the player's actor and damage it with the enemy's base damage.
					auto target_actor_id = this->find_first_player();
					if(target_actor_id.has_value() && target_actor_id.value() != id)
					{
						// Hit the player for the enemy's base damage.
						actor.damage(this->actors[target_actor_id.value()]);
					}
				}
			}
		}
		// We now know for certain whether the actor wants to move or not. Now we can finally carry out the movement.
		float sp = actor.base_movement;
		if(actor.actions.contains(ActorAction::MoveLeft))
		{
			quad.position[0] -= sp;
		}
		if(actor.actions.contains(ActorAction::MoveRight))
		{
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

		// Functionality for actors which are hazardous. They should attempt to damage anything that gets too close.
		if(actor.flags.contains(ActorFlag::HazardousToAll) || actor.flags.contains(ActorFlag::HazardousToEnemies))
		{
			
			for(std::size_t i = 0; i < this->size(); i++)
			{
				if(i == id)
				{
					continue;
				}
				if(this->actor_collision_query(id, i))
				{
					Actor& victim = this->actors[i];
					bool should_hurt = actor.flags.contains(ActorFlag::HazardousToAll) || (actor.flags.contains(ActorFlag::HazardousToEnemies) && actor.is_enemy_of(victim));
					if(should_hurt && !actor.dead() && !victim.dead())
					{
						actor.damage(victim);
					}
				}
			}
		}
		// Functionality for actors which collide with players.
		if(actor.flags.contains(ActorFlag::DeadRespawnOnPlayerTouch) || actor.flags.contains(ActorFlag::DeadResurrectOnPlayerTouch))
		{
			for(std::size_t i = 0; i < this->size(); i++)
			{
				if(i == id)
				{
					continue;
				}
				if(this->actors[i].flags.contains(ActorFlag::Player) && this->actor_collision_query(id, i))
				{
					// Actor is touching a player.
					if(actor.flags.contains(ActorFlag::DeadResurrectOnPlayerTouch))
					{
						actor.current_health = actor.max_health;
					}
					if(actor.flags.contains(ActorFlag::DeadRespawnOnPlayerTouch))
					{
						actor.respawn();
					}
				}
			}
		}
	}

	std::optional<std::size_t> Scene::find_first_player() const
	{
		// Player Finding will only target living players.
		for(std::size_t i = 0; i < this->size(); i++)
		{
			if(this->actors[i].flags.contains(ActorFlag::Player) && !this->actors[i].dead())
			{
				return {i};
			}
		}
		return std::nullopt;
	}

	bool Scene::actor_collision_query(std::size_t actor_a, std::size_t actor_b) const
	{
		const QuadRenderer::ElementData& a = this->qrenderer.elements()[actor_a];
		const QuadRenderer::ElementData& b = this->qrenderer.elements()[actor_b];

		tz::Vec2 displacement = a.position - b.position;
		return displacement.length() <= touch_distance;
	}

	bool Scene::is_in_bounds(std::size_t actor_id) const
	{
		tz::Vec2 pos = this->qrenderer.elements()[actor_id].position;
		auto bounds = this->get_world_boundaries();
		return
			bounds.first[0] <= pos[0] && pos[0] <= bounds.second[0]
		     && bounds.first[1] <= pos[1] && pos[1] <= bounds.second[1];
	}

	std::pair<tz::Vec2, tz::Vec2> Scene::get_world_boundaries() const
	{
		return
		{
			tz::Vec2{-1.0f, -1.0f}, tz::Vec2{1.0f, 1.0f}
		};
	}
}
