#include "actor.hpp"
#include "util.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "tz/wsi/keyboard.hpp"
#include "tz/core/profile.hpp"

using namespace tz::literals;

namespace game
{
	std::size_t Actor::uuid_count = 0;
	constexpr float level_transition_length = 1000.0f;

	Actor create_actor(ActorType type)
	{
		switch(type)
		{
			case ActorType::PlayerAkhara:
				#include "actors/PlayerAkhara.actor"
			break;
			case ActorType::GhostZombie:
				#include "actors/GhostZombie.actor"
			break;
			case ActorType::GhostBanshee:
				#include "actors/GhostBanshee.actor"
			break;
			case ActorType::GhostBanshee_Spirit:
				#include "actors/GhostBanshee_Spirit.actor"
			break;
			case ActorType::ChaoticFireball:
				#include "actors/ChaoticFireball.actor"
			break;
			case ActorType::Frostbolt:
				#include "actors/Frostbolt.actor"
			break;
			case ActorType::FrozenOrb:
				#include "actors/FrozenOrb.actor"
			break;
			case ActorType::FireSmoke:
				#include "actors/FireSmoke.actor"
			break;
			case ActorType::FireExplosion:
				#include "actors/FireExplosion.actor"
			break;
			case ActorType::BloodSplatter:
				#include "actors/BloodSplatter.actor"
			break;
			case ActorType::Nightmare:
				#include "actors/Nightmare.actor"
			break;
			case ActorType::GhostZombie_Spawner:
				#include "actors/GhostZombie_Spawner.actor"
			break;
			case ActorType::Fireball_Spawner:
				#include "actors/Fireball_Spawner.actor"
			break;
			case ActorType::Wall:
				#include "actors/Wall.actor"
			break;
			case ActorType::InvisibleWall:
				#include "actors/InvisibleWall.actor"
			break;
			case ActorType::InvisibleWallProjectilePass:
				#include "actors/InvisibleWallProjectilePass.actor"
			break;
			case ActorType::WallDestructible:
				#include "actors/WallDestructible.actor"
			break;
			case ActorType::Scenery_Gravestone:
				#include "actors/Scenery_Gravestone.actor"
			break;
			case ActorType::Interactable_Torch:
				#include "actors/Interactable_Torch.actor"
			break;
			case ActorType::Interactable_Stone_Stairs_Down_NX:
				#include "actors/Interactable_Stone_Stairs_Down_NX.actor"
			break;
			case ActorType::Interactable_Stone_Stairs_Up_NX:
				#include "actors/Interactable_Stone_Stairs_Up_NX.actor"
			break;
			case ActorType::Interactable_Stone_Stairs_Down_PY:
				#include "actors/Interactable_Stone_Stairs_Down_PY.actor"
			break;
			case ActorType::Interactable_Stone_Stairs_Up_PY:
				#include "actors/Interactable_Stone_Stairs_Up_PY.actor"
			break;
			case ActorType::CollectablePowerup_Sprint:
				#include "actors/CollectablePowerup_Sprint.actor"
			break;
			case ActorType::RoadSpeedBoost:
				#include "actors/RoadSpeedBoost.actor"
			break;
			case ActorType::World:
				return
				{
					.type = ActorType::World,
					.name = "World"
				};
			break;
		}
		return Actor::null();
	}

	void Actor::update()
	{
		TZ_PROFZONE("Actor - Update", 0xFF00AA00);
		this->evaluate_buffs();
		this->entity.update();
		this->evaluate_animation();
		if(this->flags.has<FlagID::BlockingAnimations>())
		{
			if(!this->animation.get_info().loop && !this->animation.complete())
			{
				return;
			}
		}
		const bool is_left_clicked = tz::wsi::is_mouse_button_down(tz::window().get_mouse_state(), tz::wsi::mouse_button::left) && !tz::dbgui::claims_mouse();
		if(!this->dead())
		{
			if(this->flags.has<FlagID::ClickToLaunch>() && is_left_clicked)
			{
				auto& flag = this->flags.get<FlagID::ClickToLaunch>()->data();
				float cooldown_milliseconds = flag.internal_cooldown * 1000;
				auto now = tz::system_time().millis<unsigned long long>();
				// If cooldown is over, go for it. Otherwise do nothing.
				const unsigned long long millis_passed = now - flag.launch_time;

				if(millis_passed > cooldown_milliseconds)
				{
					flag.launch_time = now;

					this->entity.add<ActionID::LaunchToMouse>
					({
						.speed_multiplier = 8.0f
					});
				}
				else
				{
				}
			}
			if(this->flags.has<FlagID::MouseControlled>())
			{
				this->entity.set<ActionID::GotoMouse>();
			}
			if(this->flags.has<FlagID::ActionOnClick>() && !tz::dbgui::claims_mouse())
			{
				auto& flag = this->flags.get<FlagID::ActionOnClick>()->data();
				for(auto& [button, data] : flag.action_map)
				{
					if(tz::wsi::is_mouse_button_down(tz::window().get_mouse_state(), button) && data.icd.done())
					{
						data.actions.copy_components(this->entity);
						data.icd.reset();
					}
				}
			}
			if(this->flags.has<FlagID::KeyboardControlled>() && !this->flags.has<FlagID::SuppressedControl>())
			{
				const auto& kb = tz::window().get_keyboard_state();
				this->motion = {};
				if(tz::wsi::is_key_down(kb, tz::wsi::key::w))
				{
					this->motion |= ActorMotion::MoveUp;
				}
				if(tz::wsi::is_key_down(kb, tz::wsi::key::a))
				{
					this->motion |= ActorMotion::MoveLeft;
				}
				if(tz::wsi::is_key_down(kb, tz::wsi::key::s))
				{
					this->motion |= ActorMotion::MoveDown;
				}
				if(tz::wsi::is_key_down(kb, tz::wsi::key::d))
				{
					this->motion |= ActorMotion::MoveRight;
				}
			}
		}
		else
		{
			// Actor is dead.
			if(this->flags.has<FlagID::RespawnOnClick>() && is_left_clicked)
			{
				this->respawn();
				return;
			}
		}
	}

	bool Actor::dead() const
	{
		return !this->flags.has<FlagID::Invincible>() && this->base_stats.current_health <= 0.0f;
	}

	void Actor::dbgui()
	{
		ImGui::Text("UUID: %zu", this->uuid);
		ImGui::Text("Flags New Count: %zu", this->flags.size());
		ImGui::Text("\"%s\" (HP: %.2f/%.2f, dead: %s)", this->name, this->base_stats.current_health, this->get_current_stats().max_health, this->dead() ? "true" : "false");
		ImGui::SameLine();
		if(ImGui::Button("Kill"))
		{
			this->base_stats.current_health = 0;
		}
		if(ImGui::Button("Toggle Invincibility"))
		{
			if(this->flags.has<FlagID::Invincible>())
			{
				this->flags.remove<FlagID::Invincible>();
			}
			else
			{
				this->flags.add<FlagID::Invincible>();
			}
		}
		if(ImGui::Button("Take Control"))
		{
			this->faction = Faction::PlayerFriend;
			this->flags.add<FlagID::Player>();
			this->flags.add<FlagID::KeyboardControlled>();
			this->flags.remove<FlagID::AggressiveIf>();
		}
		ImGui::Spacing();
		ImGui::Text("Movement: %.0f%%", this->get_current_stats().movement_speed / this->base_stats.movement_speed * 100.0f);
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

			ImGui::Text("Controlled: %s", this->flags.has<FlagID::MouseControlled>() ? "true" : "false");
			ImGui::SameLine();
			if(ImGui::Button("Toggle Controlled"))
			{
				if(this->flags.has<FlagID::MouseControlled>())
				{
					this->flags.remove<FlagID::MouseControlled>();
				}
				else
				{
					this->flags.add<FlagID::MouseControlled>();
				}
			}
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
	}

	void Actor::respawn()
	{
		*this = game::create_actor(this->type);
		this->base_stats.current_health = this->get_current_stats().max_health;
		if(this->flags.has<FlagID::ActionOnRespawn>())
		{
			auto& flag = this->flags.get<FlagID::ActionOnRespawn>()->data();
			flag.actions.copy_components(this->entity);
		}
	}

	Stats Actor::get_current_stats() const
	{
		Stats result = this->base_stats;
		for(const StatBuff& buff : this->buffs.elements())
		{
			for(std::size_t i = 0; i < buff.stacks; i++)
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
		}
		return result;
	}

	bool Actor::is_ally_of(const Actor& actor) const
	{
		// World is neither an ally nor enemy of anyone.
		if(this->type == ActorType::World || actor.type == ActorType::World)
		{
			return false;
		}
		// Players cannot hurt other players.
		if(this->flags.has<FlagID::Player>() && actor.flags.has<FlagID::Player>())
		{
			return true;
		}
		// PlayerFriends cannot hurt players.
		if(this->faction == Faction::PlayerFriend && actor.flags.has<FlagID::Player>())
		{
			return true;
		}
		// Players cannot hurt PlayerFriends.
		if(this->flags.has<FlagID::Player>() && actor.faction == Faction::PlayerFriend)
		{
			return true;
		}
		if(this->faction == Faction::PureEnemy && this->type != actor.type)
		{
			return false;
		}
		if(actor.faction == Faction::PureEnemy && actor.type != this->type)
		{
			return false;
		}
		if(this->faction == Faction::PureFriend)
		{
			return true;
		}
		if(actor.faction == Faction::PureFriend)
		{
			return true;
		}
		return false;
	}

	bool Actor::is_enemy_of(const Actor& actor) const
	{
		// World is neither an ally nor enemy of anyone.
		if(this->type == ActorType::World || actor.type == ActorType::World)
		{
			return false;
		}
		// Players see PlayerEnemy faction members as enemies... obviously
		if(this->flags.has<FlagID::Player>() && actor.faction == Faction::PlayerEnemy)
		{
			return true;
		}
		// PlayerEnemy faction members see Players as enemies.
		if(this->faction == Faction::PlayerEnemy && actor.flags.has<FlagID::Player>())
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
		if(actor.faction == Faction::PureEnemy && actor.type != this->type)
		{
			return true;
		}
		if(this->faction == Faction::PureFriend)
		{
			return false;
		}
		if(actor.faction == Faction::PureFriend)
		{
			return false;
		}
		return false;
	}

	void Actor::evaluate_buffs()
	{
		TZ_PROFZONE("Actor - Buff Evaluation", 0xFF00AA00);
		auto now = tz::system_time();
		auto time_since_update = now - last_update;
		for(int i = 0; i < this->buffs.elements().size(); i++)
		{
			auto& buff = this->buffs.elements()[i];
			if(buff.time_remaining_millis > time_since_update.millis<float>())
			{
				buff.time_remaining_millis -= time_since_update.millis<float>();
			}
			else
			{
				this->buffs.erase(i--);
			}
		}
	}

	void Actor::evaluate_animation()
	{
		TZ_PROFZONE("Actor - Animation Evaluation", 0xFF00AA00);
		AnimationID ending_animation = AnimationID::Missing;
		switch(this->skin)
		{
			case ActorSkin::PlayerAkhara_DefaultFireball:
				ending_animation = AnimationID::PlayerAkhara_DefaultFireball_Idle;
			break;
			case ActorSkin::Material_Stone:
				if(this->dead())
				{
					ending_animation = AnimationID::BlockBreak;
				}
				else
				{
					ending_animation = AnimationID::Material_Stone;
				}
			break;
			case ActorSkin::Gui_Healthbar:
				ending_animation = AnimationID::Gui_Healthbar_Empty;
			break;
			case ActorSkin::Scenery_Gravestone_0:
				if(this->dead())
				{
					ending_animation = AnimationID::BlockBreak;
				}
				else
				{
					ending_animation = AnimationID::Scenery_Gravestone_0;
				}
			break;
			case ActorSkin::Scenery_Gravestone_1:
				if(this->dead())
				{
					ending_animation = AnimationID::BlockBreak;
				}
				else
				{
					ending_animation = AnimationID::Scenery_Gravestone_1;
				}
			break;
			case ActorSkin::Interactable_Torch:
				ending_animation = AnimationID::Interactable_Torch_Off;
			break;
			case ActorSkin::Interactable_Stone_Stairs_Down_NX:
				if(this->dead())
				{
					ending_animation = AnimationID::Downwards_Trapdoor;
				}
				else
				{
					ending_animation = AnimationID::Interactable_Stone_Stairs_Down_NX;
				}
			break;
			case ActorSkin::Interactable_Stone_Stairs_Up_NX:
				ending_animation = AnimationID::Interactable_Stone_Stairs_Up_NX;
			break;
			case ActorSkin::Interactable_Stone_Stairs_Down_PY:
				if(this->dead())
				{
					ending_animation = AnimationID::Downwards_Trapdoor;
				}
				else
				{
					ending_animation = AnimationID::Interactable_Stone_Stairs_Down_PY;
				}
			break;
			case ActorSkin::Interactable_Stone_Stairs_Up_PY:
				ending_animation = AnimationID::Interactable_Stone_Stairs_Up_PY;
			break;
			case ActorSkin::BloodSplatter:
				ending_animation = AnimationID::BloodSplatter;
			break;
			case ActorSkin::Powerup_Sprint:
				ending_animation = AnimationID::Powerup_Sprint;
			break;
			case ActorSkin::FireSmoke:
				ending_animation = AnimationID::PlayerAkhara_FireSmoke;
			break;
			case ActorSkin::FireExplosion:
				ending_animation = AnimationID::PlayerAkhara_FireExplosion;
			break;
			case ActorSkin::PlayerAkhara_LowPoly:
				if(this->motion.contains(ActorMotion::MoveLeft))
				{
					ending_animation = AnimationID::PlayerAkhara_LowPoly_MoveSide;
				}
				else if(this->motion.contains(ActorMotion::MoveRight))
				{
					ending_animation = AnimationID::PlayerAkhara_LowPoly_MoveSide;
					this->entity.add<ActionID::HorizontalFlip>();
				}
				else if(this->motion.contains(ActorMotion::MoveUp))
				{
					ending_animation = AnimationID::PlayerAkhara_LowPoly_MoveUp;
				}
				else if(this->motion.contains(ActorMotion::MoveDown))
				{
					ending_animation = AnimationID::PlayerAkhara_LowPoly_MoveDown;
				}
				else
				{
					ending_animation = AnimationID::PlayerAkhara_LowPoly_Idle;
				}
				if(this->dead())
				{
					// TODO: Death animation for LowPoly
					ending_animation = AnimationID::PlayerAkhara_LowPoly_Death;
				}
				if(this->entity.has<ActionID::Cast>())
				{
					const auto& action = this->entity.get<ActionID::Cast>()->data();
					const Animation end_cast_anim = game::play_animation(AnimationID::PlayerAkhara_LowPoly_EndCast);
					const int end_cast_anim_time_millis = (static_cast<float>(end_cast_anim.get_info().frames.size()) / end_cast_anim.get_info().fps) * 1000.0f;
					if(action.cast_time_millis <= end_cast_anim_time_millis)
					{
						ending_animation = AnimationID::PlayerAkhara_LowPoly_EndCast;
					}
					else
					{
						ending_animation = AnimationID::PlayerAkhara_LowPoly_Cast;
					}
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
			case ActorSkin::GhostMJZombie:
				if(this->motion.contains(ActorMotion::MoveLeft))
				{
					ending_animation = AnimationID::GhostMJZombie_MoveSide;
				}
				else if(this->motion.contains(ActorMotion::MoveRight))
				{
					ending_animation = AnimationID::GhostMJZombie_MoveSide;
					this->entity.add<ActionID::HorizontalFlip>();
				}
				else if(this->motion.contains(ActorMotion::MoveUp))
				{
					ending_animation = AnimationID::GhostMJZombie_MoveUp;
				}
				else if(this->motion.contains(ActorMotion::MoveDown))
				{
					ending_animation = AnimationID::GhostMJZombie_MoveDown;
				}
				else
				{
					ending_animation = AnimationID::GhostMJZombie_Idle;
				}
				if(this->dead())
				{
					ending_animation = AnimationID::GhostMJZombie_Death;
				}
			break;
			case ActorSkin::GhostBanshee:
				if(this->dead())
				{
					ending_animation = AnimationID::GhostBanshee_Death;
				}
				else
				{
					ending_animation = AnimationID::GhostBanshee_Idle;
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
			case ActorSkin::Invisible:
				ending_animation = AnimationID::Invisible;
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
		float speed_ratio = this->get_current_stats().movement_speed / this->base_stats.movement_speed;
		// We want multiplyer to be half of the % movement speed.
		this->animation.set_fps_multiplyer((1.0f + speed_ratio) / 2.0f);
	}
}
