#include "actor.hpp"
#include "util.hpp"
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
					.base_stats =
					{
						.movement_speed = 0.001f
					},
					.skin = ActorSkin::PlayerClassic,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle)
				};
			break;
			case ActorType::PlayerClassic_TestEvil:
				return
				{
					.type = ActorType::PlayerClassic_TestEvil,
					.flags = {ActorFlag::Aggressive},
					.faction = Faction::PlayerEnemy,
					.base_stats =
					{
						.movement_speed = 0.0005f,
						.max_health = 0.01f,
						.current_health = 0.01f
					},
					.skin = ActorSkin::PlayerClassic,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle)
				};
			break;
			case ActorType::PlayerClassic_Orb:
				return
				{
					.type = ActorType::PlayerClassic_Orb,
					.flags = {ActorFlag::HazardousToEnemies, ActorFlag::ClickToLaunch, ActorFlag::RespawnOnPlayer, ActorFlag::DieIfOOB, ActorFlag::RespawnOnClick, ActorFlag::SelfHarm, ActorFlag::InvisibleWhileDead},
					.faction = Faction::PlayerFriend,
					.base_stats =
					{
						.movement_speed = 0.001f,
						.damage = default_base_damage * 2.0f,
						.max_health = 0.0001f,
						.current_health = 0.0f
					},
					.skin = ActorSkin::PlayerClassic_DefaultFireball,
					.animation = game::play_animation(AnimationID::PlayerClassic_DefaultFireball_Idle)
				};
			break;
			case ActorType::Nightmare:
				return
				{
					.type = ActorType::Nightmare,
					.flags = {ActorFlag::Aggressive, ActorFlag::SelfHarm, ActorFlag::RespawnOnDeath, ActorFlag::RandomRespawnLocation},
					.faction = Faction::PlayerEnemy,
					.base_stats =
					{
						.movement_speed = 0.0014f,
						.damage = 1.0f,
						.max_health = 0.001f,
						.current_health = 0.001f
					},
					.skin = ActorSkin::Nightmare,
					.actions = {ActorAction::AnimationPause},
					.animation = game::play_animation(AnimationID::Nightmare_Spawn)
				};
			break;
			case ActorType::EvilPlayer_TestSpawner:
				return
				{
					.type = ActorType::EvilPlayer_TestSpawner,
					.flags = {ActorFlag::RandomRespawnLocation, ActorFlag::RespawnOnDeath, ActorFlag::Rot, ActorFlag::Haunted},
					.skin = ActorSkin::DebugOnlyVisible
				};
			break;
		}
		return Actor::null();
	}

	void Actor::update()
	{
		this->refresh_actions();
		this->entity.update();
		if(this->actions.contains(ActorAction::AnimationPause))
		{
			if(this->animation.complete())
			{
				this->actions.remove(ActorAction::AnimationPause);
			}
			return;
		}
		const bool is_left_clicked = tz::window().get_mouse_button_state().is_mouse_button_down(tz::MouseButton::Left) && !tz::dbgui::claims_mouse();
		if(!this->dead())
		{
			if(this->flags.contains(ActorFlag::Rot))
			{
				this->damage(*this);
			}
			if(this->flags.contains(ActorFlag::ClickToLaunch) && is_left_clicked)
			{
				this->entity.add<ActionID::LaunchToMouse>
				({
					.speed_multiplier = 8.0f
				});
				this->flags |= {ActorFlag::DieAtRest};
			}
			if(this->flags.contains(ActorFlag::Aggressive) && this->faction == Faction::PlayerEnemy)
			{
				// If it wants to chase the player the whole time, let it!
				this->actions |= ActorAction::ChasePlayer;
			}
			if(this->flags.contains(ActorFlag::MouseControlled))
			{
				this->entity.set<ActionID::GotoMouse>();
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
			if(this->flags.contains(ActorFlag::RespawnOnClick) && is_left_clicked)
			{
				this->respawn();
				return;
			}
			if(this->flags.contains(ActorFlag::RespawnOnDeath))
			{
				// Wait for its death animation to finish.
				if(this->animation.complete() || this->animation.get_info().loop)
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
		return !this->flags.contains(ActorFlag::Invincible) && this->base_stats.current_health <= 0.0f;
	}

	void Actor::dbgui()
	{
		ImGui::Text("Health: %.2f/%f (dead: %s)", this->base_stats.current_health, this->get_current_stats().max_health, this->dead() ? "true" : "false");
		if(ImGui::CollapsingHeader("Current Stats"))
		{
			this->get_current_stats().dbgui();
			if(ImGui::CollapsingHeader("Debug Buffs:"))
			{
				if(ImGui::Button("Berserk"))
				{
					this->buffs.add(game::get_buff(BuffID::Berserk));
				}
				if(ImGui::Button("Sprint"))
				{
					this->buffs.add(game::get_buff(BuffID::Sprint));
				}
			}
		}
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
		ImGui::Text("Movement Speed: %.5f", this->get_current_stats().movement_speed);
		ImGui::Text("Entity Action Count: %zu", this->entity.size());
		if(ImGui::Button("Kill"))
		{
			this->base_stats.current_health = 0;
		}
	}

	void Actor::damage(Actor& victim)
	{
		// Actors cannot damage its allies.
		if(this->is_ally_of(victim))
		{
			return;
		}
		victim.base_stats.current_health -= this->get_current_stats().damage;
		if(this->flags.contains(ActorFlag::SelfHarm))
		{
			this->base_stats.current_health -= this->get_current_stats().damage;
		}
	}

	void Actor::respawn()
	{
		bool should_spawn_randomly = this->flags.contains(ActorFlag::RandomRespawnLocation);
		bool should_spawn_on_player = this->flags.contains(ActorFlag::RespawnOnPlayer);
		*this = game::create_actor(this->type);
		this->base_stats.current_health = this->get_current_stats().max_health;
		if(should_spawn_randomly)
		{
			this->entity.add<ActionID::RandomTeleport>();
		}
		else if(should_spawn_on_player)
		{
			this->entity.add<ActionID::TeleportToPlayer>();
		}
	}

	Stats Actor::get_current_stats() const
	{
		Stats result = this->base_stats;
		for(const StatBuff& buff : this->buffs)
		{
			result.movement_speed += buff.add_speed_boost;
			result.movement_speed *= buff.multiply_speed_boost;
			result.damage += buff.add_damage;
			result.damage *= buff.multiply_damage;
			result.defense += buff.add_defense;
			result.defense *= buff.multiply_defense;
			result.max_health += buff.add_health;
			result.max_health *= buff.multiply_health;
		}
		return result;
	}

	bool Actor::is_ally_of(const Actor& actor) const
	{
		// Players cannot hurt other players.
		if(this->flags.contains(ActorFlag::Player) && actor.flags.contains(ActorFlag::Player))
		{
			return true;
		}
		// PlayerFriends cannot hurt players.
		if(this->faction == Faction::PlayerFriend && actor.flags.contains(ActorFlag::Player))
		{
			return true;
		}
		// Players cannot hurt PlayerFriends.
		if(this->flags.contains(ActorFlag::Player) && actor.faction == Faction::PlayerFriend)
		{
			return true;
		}
		return false;
	}

	bool Actor::is_enemy_of(const Actor& actor) const
	{
		// Players see PlayerEnemy faction members as enemies... obviously
		if(this->flags.contains(ActorFlag::Player) && actor.faction == Faction::PlayerEnemy)
		{
			return true;
		}
		// PlayerEnemy faction members see Players as enemies.
		if(this->faction == Faction::PlayerEnemy && actor.flags.contains(ActorFlag::Player))
		{
			return true;	
		}
		// PlayerFriend faction members see PlayerEnemy faction members as enemies.
		if(this->faction == Faction::PlayerFriend && actor.faction == Faction::PlayerEnemy)
		{
			return true;
		}
		// PlayerEnemy faction members see PlayerFriend faction members as enemies.
		if(this->faction == Faction::PlayerEnemy && actor.faction == Faction::PlayerFriend)
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
					this->entity.add<ActionID::HorizontalFlip>();
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
					this->entity.add<ActionID::HorizontalFlip>();
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
			case ActorSkin::DebugOnlyVisible:
				#if TZ_DEBUG
					ending_animation = AnimationID::Missing;
				#else
					ending_animation = AnimationID::Invisible;
				#endif
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
