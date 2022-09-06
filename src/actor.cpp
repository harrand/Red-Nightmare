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
					.skin = ActorSkin::PlayerClassic_LowPoly,
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
					.skin = ActorSkin::GhostZombie,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle),
					.palette_colour = {0u, 255u, 0u}
				};
			break;
			case ActorType::PlayerClassic_Orb:
				return
				{
					.type = ActorType::PlayerClassic_Orb,
					.flags = {ActorFlag::HazardousToEnemies, ActorFlag::ClickToLaunch, ActorFlag::RespawnOnPlayer, ActorFlag::DieIfOOB, ActorFlag::RespawnOnClick, ActorFlag::SelfHarm, ActorFlag::InvisibleWhileDead, ActorFlag::DoNotGarbageCollect, ActorFlag::ExplodeOnDeath, ActorFlag::Stealth},
					.faction = Faction::PlayerFriend,
					.base_stats =
					{
						.movement_speed = 0.001f,
						.damage = default_base_damage / 2.0f,
						.max_health = 0.0001f,
						.current_health = 0.0f
					},
					.skin = ActorSkin::PlayerClassic_DefaultFireball,
					.animation = game::play_animation(AnimationID::PlayerClassic_DefaultFireball_Idle)
				};
			break;
			case ActorType::FireSmoke:
				return
				{
					.type = ActorType::FireSmoke,
					.flags = {ActorFlag::Rot, ActorFlag::BlockingAnimations, ActorFlag::InvisibleWhileDead},
					.base_stats =
					{
						.max_health = 0.1f,
						.current_health = 0.01f
					},
					.skin = ActorSkin::FireSmoke
				};
			break;
			case ActorType::FireExplosion:
				return
				{
					.type = ActorType::FireExplosion,
					.flags = {ActorFlag::Rot, ActorFlag::BlockingAnimations, ActorFlag::InvisibleWhileDead, ActorFlag::HazardousToEnemies, ActorFlag::LargeSprite, ActorFlag::HighReach, ActorFlag::Stealth},
					.base_stats =
					{
						.max_health = 0.1f,
						.current_health = 0.01f
					},
					.skin = ActorSkin::FireExplosion
				};
			break;
			case ActorType::Nightmare:
				return
				{
					.type = ActorType::Nightmare,
					.flags = {ActorFlag::Aggressive, ActorFlag::SelfHarm, ActorFlag::RespawnOnDeath, ActorFlag::RandomRespawnLocation, ActorFlag::BlockingAnimations},
					.faction = Faction::PureEnemy,
					.base_stats =
					{
						.movement_speed = 0.0014f,
						.damage = 1.0f,
						.max_health = 0.001f,
						.current_health = 0.001f
					},
					.skin = ActorSkin::Nightmare,
					.palette_colour = {64u, 32u, 8u}
				};
			break;
			case ActorType::EvilPlayer_TestSpawner:
				return
				{
					.type = ActorType::EvilPlayer_TestSpawner,
					.flags = {ActorFlag::RandomRespawnLocation, ActorFlag::RespawnOnDeath, ActorFlag::Rot, ActorFlag::Haunted, ActorFlag::Stealth},
					.skin = ActorSkin::DebugOnlyVisible,
					.palette_colour = {255u, 64u, 255u}
				};
			break;
		}
		return Actor::null();
	}

	void Actor::update()
	{
		this->entity.update();
		this->evaluate_animation();
		if(this->flags.contains(ActorFlag::BlockingAnimations))
		{
			if(!this->animation.get_info().loop && !this->animation.complete())
			{
				return;
			}
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
			if(this->flags.contains(ActorFlag::MouseControlled))
			{
				this->entity.set<ActionID::GotoMouse>();
			}
			if(this->flags.contains(ActorFlag::KeyboardControlled))
			{
				const auto& kb = tz::window().get_keyboard_state();
				this->motion = {};
				if(kb.is_key_down(tz::KeyCode::W))
				{
					this->motion |= ActorMotion::MoveUp;
				}
				if(kb.is_key_down(tz::KeyCode::A))
				{
					this->motion |= ActorMotion::MoveLeft;
				}
				if(kb.is_key_down(tz::KeyCode::S))
				{
					this->motion |= ActorMotion::MoveDown;
				}
				if(kb.is_key_down(tz::KeyCode::D))
				{
					this->motion |= ActorMotion::MoveRight;
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
			if(this->flags.contains(ActorFlag::ExplodeOnDeath))
			{
				this->entity.add<ActionID::SpawnActor>
				({
					.actor = ActorType::FireExplosion,
					.inherit_faction = true
				});
				this->flags.remove(ActorFlag::ExplodeOnDeath);
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
					this->buffs.add(BuffID::Berserk);
				}
				if(ImGui::Button("Sprint"))
				{
					this->buffs.add(BuffID::Sprint);
				}
				if(ImGui::Button("Chill"))
				{
					this->buffs.add(BuffID::Chill);
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
		for(const StatBuff& buff : this->buffs.elements())
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
		if(this->faction == Faction::PureEnemy && this->type != actor.type)
		{
			return false;
		}
		if(this->faction == Faction::PureFriend)
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
		if(this->faction == Faction::PureEnemy && this->type != actor.type)
		{
			return true;
		}
		if(this->faction == Faction::PureFriend)
		{
			return false;
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
			case ActorSkin::FireSmoke:
				ending_animation = AnimationID::PlayerClassic_FireSmoke;
			break;
			case ActorSkin::FireExplosion:
				ending_animation = AnimationID::PlayerClassic_FireExplosion;
			break;
			case ActorSkin::PlayerClassic:
				if(this->motion.contains(ActorMotion::MoveLeft))
				{
					ending_animation = AnimationID::PlayerClassic_MoveSide;
				}
				else if(this->motion.contains(ActorMotion::MoveRight))
				{
					ending_animation = AnimationID::PlayerClassic_MoveSide;
					this->entity.add<ActionID::HorizontalFlip>();
				}
				else if(this->motion.contains(ActorMotion::MoveUp))
				{
					ending_animation = AnimationID::PlayerClassic_MoveUp;
				}
				else if(this->motion.contains(ActorMotion::MoveDown))
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
			case ActorSkin::PlayerClassic_LowPoly:
				if(this->motion.contains(ActorMotion::MoveLeft))
				{
					ending_animation = AnimationID::PlayerClassic_LowPoly_MoveSide;
				}
				else if(this->motion.contains(ActorMotion::MoveRight))
				{
					ending_animation = AnimationID::PlayerClassic_LowPoly_MoveSide;
					this->entity.add<ActionID::HorizontalFlip>();
				}
				else if(this->motion.contains(ActorMotion::MoveUp))
				{
					ending_animation = AnimationID::PlayerClassic_LowPoly_MoveUp;
				}
				else if(this->motion.contains(ActorMotion::MoveDown))
				{
					ending_animation = AnimationID::PlayerClassic_LowPoly_MoveDown;
				}
				else
				{
					ending_animation = AnimationID::PlayerClassic_LowPoly_Idle;
				}
				if(this->dead())
				{
					// TODO: Death animation for LowPoly
					ending_animation = AnimationID::PlayerClassic_Death;
				}
			break;
			case ActorSkin::GhostZombie:
				if(this->motion.contains(ActorMotion::MoveLeft))
				{
					ending_animation = AnimationID::GhostZombie_MoveSide;
				}
				else if(this->motion.contains(ActorMotion::MoveRight))
				{
					ending_animation = AnimationID::GhostZombie_MoveSide;
					this->entity.add<ActionID::HorizontalFlip>();
				}
				else if(this->motion.contains(ActorMotion::MoveUp))
				{
					ending_animation = AnimationID::GhostZombie_MoveUp;
				}
				else if(this->motion.contains(ActorMotion::MoveDown))
				{
					ending_animation = AnimationID::GhostZombie_MoveDown;
				}
				else
				{
					ending_animation = AnimationID::GhostZombie_Idle;
				}
				if(this->dead())
				{
					ending_animation = AnimationID::GhostZombie_Death;
				}
			break;
			case ActorSkin::Nightmare:
				if(this->motion.contains(ActorMotion::MoveLeft))
				{
					ending_animation = AnimationID::Nightmare_MoveSide;
				}
				else if(this->motion.contains(ActorMotion::MoveRight))
				{
					ending_animation = AnimationID::Nightmare_MoveSide;
					this->entity.add<ActionID::HorizontalFlip>();
				}
				else if(this->motion.contains(ActorMotion::MoveUp))
				{
					ending_animation = AnimationID::Nightmare_MoveUp;
				}
				else if(this->motion.contains(ActorMotion::MoveDown))
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
}
