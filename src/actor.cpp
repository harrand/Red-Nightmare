#include "actor.hpp"
#include "tz/core/tz.hpp"
#include "tz/dbgui/dbgui.hpp"

namespace game
{
	Actor create_actor(ActorType type)
	{
		switch(type)
		{
			case ActorType::PlayerClassic:
				return
				{
					.type = ActorType::PlayerClassic,
					.flags = {ActorFlag::Player, ActorFlag::KeyboardControlled},
					.base_movement = 0.001f,
					.skin = ActorSkin::PlayerClassic,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle)
				};
			break;
			case ActorType::PlayerClassic_TestEvil:
				return
				{
					.type = ActorType::PlayerClassic_TestEvil,
					.flags = {ActorFlag::HostileGhost},
					.base_movement = 0.0005f,
					.max_health = 0.1f,
					.current_health = 0.1f,
					.skin = ActorSkin::PlayerClassic,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle)
				};
			break;
			case ActorType::PlayerClassic_Orb:
				return
				{
					.type = ActorType::PlayerClassic_Orb,
					.flags = {ActorFlag::HostileGhost, ActorFlag::Ally, ActorFlag::Hazardous, ActorFlag::ClickToLaunch, ActorFlag::DeadRespawnOnPlayerTouch, ActorFlag::Identitarian},
					.base_movement = 0.001f,
					.base_damage = default_base_damage * 2.0f,
					.max_health = 0.1f,
					.current_health = 0.1f,
					.skin = ActorSkin::PlayerClassic_DefaultFireball,
					.animation = game::play_animation(AnimationID::PlayerClassic_DefaultFireball_Idle)
				};
			break;
			case ActorType::Nightmare:
				return
				{
					.type = ActorType::Nightmare,
					.flags = {ActorFlag::HostileGhost, ActorFlag::SelfHarm, ActorFlag::RespawnOnDeath, ActorFlag::RandomRespawnLocation},
					.base_movement = 0.0014f,
					.base_damage = 1.0f,
					.max_health = 0.001,
					.current_health = 0.001,
					.skin = ActorSkin::Nightmare,
					.actions = {ActorAction::AnimationPause},
					.animation = game::play_animation(AnimationID::Nightmare_Spawn)
				};
			break;
		}
		return Actor::null();
	}

	void Actor::update()
	{
		this->refresh_actions();
		if(this->actions.contains(ActorAction::AnimationPause))
		{
			if(this->animation.complete())
			{
				this->actions.remove(ActorAction::AnimationPause);
			}
			return;
		}
		if(!this->dead())
		{
			if(this->flags.contains(ActorFlag::ClickToLaunch) && tz::window().get_mouse_button_state().is_mouse_button_down(tz::MouseButton::Left) && !tz::dbgui::claims_mouse())
			{
				this->flags |= {ActorFlag::ChaseMouse, ActorFlag::FastUntilRest, ActorFlag::DieAtRest};
			}
			if(this->flags.contains(ActorFlag::HostileGhost))
			{
				// If it wants to chase the player the whole time, let it!
				this->actions |= ActorAction::ChasePlayer;
			}
			if(this->flags.contains(ActorFlag::MouseControlled) || this->flags.contains(ActorFlag::ChaseMouse))
			{
				this->actions |= ActorAction::FollowMouse;
			}
			if(this->flags.contains(ActorFlag::KeyboardControlled))
			{
				const auto& kb = tz::window().get_keyboard_state();
				bool should_move = false;
				if(kb.is_key_down(tz::KeyCode::W))
				{
					this->actions |= ActorAction::MoveUp;
				}
				if(kb.is_key_down(tz::KeyCode::A))
				{
					this->actions |= ActorAction::MoveLeft;
				}
				if(kb.is_key_down(tz::KeyCode::S))
				{
					this->actions |= ActorAction::MoveDown;
				}
				if(kb.is_key_down(tz::KeyCode::D))
				{
					this->actions |= ActorAction::MoveRight;
				}
			}
		}
		else
		{
			// Actor is dead.
			if(this->flags.contains(ActorFlag::RespawnOnDeath))
			{
				// Wait for its death animation to finish.
				if(this->animation.complete())
				{
					this->respawn();
					return;
				}
			}
		}
		this->evaluate_animation();
	}

	bool Actor::dead() const
	{
		return !this->flags.contains(ActorFlag::Invincible) && this->current_health <= 0.0f;
	}

	void Actor::dbgui()
	{
		ImGui::Text("Health: %.2f/%f (dead: %s)", this->current_health, this->max_health, this->dead() ? "true" : "false");
		ImGui::Text("Invincible: %s", this->flags.contains(ActorFlag::Invincible) ? "true" : "false");
		ImGui::SameLine();
		if(ImGui::Button("Toggle Invincible"))
		{
			if(this->flags.contains(ActorFlag::Invincible))
			{
				this->flags.remove(ActorFlag::Invincible);
			}
			else
			{
				this->flags |= ActorFlag::Invincible;
			}
		}
		ImGui::Text("Controlled: %s", this->flags.contains(ActorFlag::MouseControlled) ? "true" : "false");
		ImGui::SameLine();
		if(ImGui::Button("Toggle Controlled"))
		{
			if(this->flags.contains(ActorFlag::MouseControlled))
			{
				this->flags.remove(ActorFlag::MouseControlled);
			}
			else
			{
				this->flags |= ActorFlag::MouseControlled;
			}
		}
		ImGui::Text("Base Movement Speed: %.5f", this->base_movement);
		if(ImGui::Button("Kill"))
		{
			this->current_health = 0;
		}
	}

	void Actor::damage(Actor& victim)
	{
		// Actors cannot damage its allies.
		if(this->is_ally_of(victim))
		{
			return;
		}
		victim.current_health -= this->base_damage;
		if(this->flags.contains(ActorFlag::SelfHarm))
		{
			this->current_health -= this->base_damage;
		}
	}

	void Actor::respawn()
	{
		bool should_spawn_randomly = this->flags.contains(ActorFlag::RandomRespawnLocation);
		*this = game::create_actor(this->type);
		if(should_spawn_randomly)
		{
			this->actions |= ActorAction::RandomTeleport;
		}
	}

	bool Actor::is_ally_of(const Actor& actor) const
	{
		// Players cannot hurt other players.
		if(this->flags.contains(ActorFlag::Player) && actor.flags.contains(ActorFlag::Player))
		{
			return true;
		}
		// Allies cannot hurt players.
		if(this->flags.contains(ActorFlag::Ally) && actor.flags.contains(ActorFlag::Player))
		{
			return true;
		}
		// Players cannot hurt allies.
		if(this->flags.contains(ActorFlag::Player) && actor.flags.contains(ActorFlag::Ally))
		{
			return true;
		}
		// Identitarians are allies of their own kind.
		if(this->flags.contains(ActorFlag::Identitarian) && this->type == actor.type)
		{
			return true;
		}
		return false;
	}

	void Actor::evaluate_animation()
	{
		AnimationID ending_animation = AnimationID::Missing;
		switch(this->skin)
		{
			case ActorSkin::PlayerClassic_DefaultFireball:
				ending_animation = AnimationID::PlayerClassic_DefaultFireball_Idle;
			break;
			case ActorSkin::PlayerClassic:
				if(this->actions.contains(ActorAction::MoveLeft))
				{
					ending_animation = AnimationID::PlayerClassic_MoveSide;
				}
				else if(this->actions.contains(ActorAction::MoveRight))
				{
					ending_animation = AnimationID::PlayerClassic_MoveSide;
					this->actions |= ActorAction::HorizontalFlip;
				}
				else if(this->actions.contains(ActorAction::MoveUp))
				{
					ending_animation = AnimationID::PlayerClassic_MoveUp;
				}
				else if(this->actions.contains(ActorAction::MoveDown))
				{
					ending_animation = AnimationID::PlayerClassic_MoveDown;
				}
				else
				{
					ending_animation = AnimationID::PlayerClassic_Idle;
				}
				if(this->dead())
				{
					ending_animation = AnimationID::PlayerClassic_Death;
				}
			break;
			case ActorSkin::Nightmare:
				if(this->actions.contains(ActorAction::MoveLeft))
				{
					ending_animation = AnimationID::Nightmare_MoveSide;
				}
				else if(this->actions.contains(ActorAction::MoveRight))
				{
					ending_animation = AnimationID::Nightmare_MoveSide;
					this->actions |= ActorAction::HorizontalFlip;
				}
				else if(this->actions.contains(ActorAction::MoveUp))
				{
					ending_animation = AnimationID::Nightmare_MoveUp;
				}
				else if(this->actions.contains(ActorAction::MoveDown))
				{
					ending_animation = AnimationID::Nightmare_MoveDown;
				}
				else
				{
					ending_animation = AnimationID::Nightmare_Idle;
				}
				if(this->dead())
				{
					ending_animation = AnimationID::Nightmare_Death;
				}
			break;
		}
		this->assign_animation(ending_animation);
	}

	void Actor::assign_animation(AnimationID id)
	{
		Animation anim = game::play_animation(id);
		if(this->animation != anim)
		{
			this->animation = anim;
		}
	}

	void Actor::assign_blocking_animation(AnimationID id)
	{
		tz_assert(!game::play_animation(id).get_info().loop, "Assigning a blocking animation to an actor is fine, but the animation is looped, meaning the actor will be blocked forever.");
		this->assign_animation(id);
		this->actions |= ActorAction::AnimationPause;
	}

	void Actor::refresh_actions()
	{
		ActorActions preserved_actions;
		if(this->actions.contains(ActorAction::AnimationPause))
		{
			preserved_actions |= ActorAction::AnimationPause;
		}
		if(this->actions.contains(ActorAction::SceneMessage_MaintainMotion))
		{
			// Figure out which motion we were going in, and preserve it.
			constexpr std::array<ActorAction, 4> move_actions
			{
				ActorAction::MoveLeft,
				ActorAction::MoveRight,
				ActorAction::MoveDown,
				ActorAction::MoveUp
			};
			for(auto action : move_actions)
			{
				if(this->actions.contains(action))
				{
					preserved_actions |= action;
				}
			}
		}
		this->actions = {};
		for(auto action : preserved_actions)
		{
			this->actions |= action;
		}
	}
}
