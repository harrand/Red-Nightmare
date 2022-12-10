#include "actor.hpp"
#include "util.hpp"
#include "tz/core/tz.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "tz/core/profiling/zone.hpp"

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
					.flags_new =
					{
						Flag<FlagID::KeyboardControlled>{},
						Flag<FlagID::Player>{},
						Flag<FlagID::ActionOnStruck>
						{{
							.actions =
							{
								Action<ActionID::SpawnActor>
								{{
									.actor = ActorType::BloodSplatter
								}}
							},
							.icd = 200.0f
						}}
					},
					.base_stats =
					{
						.movement_speed = 0.0016f
					},
					.skin = ActorSkin::PlayerClassic_LowPoly,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle),
					.name = "PlayerClassic"
				};
			break;
			case ActorType::GhostZombie:
				return
				{
					.type = ActorType::GhostZombie,
					.flags_new =
					{
						Flag<FlagID::Collide>
						{{
							.collision_filter =
							{
								ActorType::GhostZombie,
								ActorType::GhostBanshee
							}
						}},
						Flag<FlagID::Aggressive>{},
						Flag<FlagID::HazardousIf>
						{{
							.predicate = [](const Actor& self, const Actor& victim){return self.is_enemy_of(victim);}
						}},
						Flag<FlagID::RandomSkin>
						{{
							.skins =
							{
								ActorSkin::GhostZombie,
								ActorSkin::GhostZombie,
								ActorSkin::GhostZombie,
								ActorSkin::GhostZombie,
								ActorSkin::GhostZombie,
								ActorSkin::GhostZombie,
								ActorSkin::GhostZombie,
								ActorSkin::GhostZombie,
								ActorSkin::GhostZombie,
								ActorSkin::GhostMJZombie
							}		
						}},
						Flag<FlagID::ActionOnStruck>
						{{
							.actions =
							{
								Action<ActionID::SpawnActor>
								{{
									.actor = ActorType::BloodSplatter
								}}
							},
							.icd = 200.0f
						}},
						Flag<FlagID::WanderIfIdle>{}
					},
					.faction = Faction::PlayerEnemy,
					.base_stats =
					{
						.movement_speed = 0.0008f,
						.max_health = 0.01f,
						.current_health = 0.01f
					},
					.skin = ActorSkin::GhostZombie,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle),
					.palette_colour = {0u, 255u, 0u},
					.name = "Ghost Zombie"
				};
			break;
			case ActorType::GhostBanshee:
				return
				{
					.type = ActorType::GhostBanshee,
					.flags_new =
					{
						Flag<FlagID::Collide>
						{{
							.collision_filter =
							{
								ActorType::GhostZombie,
								ActorType::GhostBanshee
							}
						}},
						Flag<FlagID::Aggressive>{},
						Flag<FlagID::HazardousIf>
						{{
							.predicate = [](const Actor& self, const Actor& victim){return self.is_enemy_of(victim);}
						}},
						Flag<FlagID::ActionOnHit>
						{{
							.actions =
							{
								Action<ActionID::ApplyBuffToTarget>
								{{
									.buff = BuffID::Chill
								}}
							}
						}}
					},
					.faction = Faction::PlayerEnemy,
					.base_stats =
					{
						.movement_speed = 0.0002f,
						.max_health = 0.01f,
						.current_health = 0.01f
					},
					.skin = ActorSkin::GhostBanshee,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle),
					.palette_colour = {128u, 255u, 255u},
					.name = "Ghost Banshee"
				};
			break;
			case ActorType::PlayerClassic_Orb:
				return
				{
					.type = ActorType::PlayerClassic_Orb,
					.flags_new =
					{
						Flag<FlagID::InvisibleWhileDead>{},
						Flag<FlagID::DoNotGarbageCollect>{},
						Flag<FlagID::CustomScale>{{.scale = {0.65f, 0.65f}}},
						Flag<FlagID::ActionOnOOB>
						{{
							.actions =
							{
								Action<ActionID::Die>{{}}
							}
						}},
						Flag<FlagID::HazardousIf>
						{{
							.predicate = [](const Actor& self, const Actor& actor)->bool
							{
								return self.is_enemy_of(actor);
							}
						}},
						Flag<FlagID::ClickToLaunch>
						{{
							.internal_cooldown = 1.0f
						}},
						Flag<FlagID::Stealth>{},
						Flag<FlagID::Unhittable>{},
						Flag<FlagID::SelfRecoil>{},
						Flag<FlagID::RespawnOnClick>{},
						Flag<FlagID::ActionOnDeath>
						{{
							.actions =
							{
								Action<ActionID::SpawnActor>
								{{
									.actor = ActorType::FireExplosion,
									.inherit_faction = true
								}},
								Action<ActionID::SpawnActor>
								{{
									.actor = ActorType::FireSmoke,
									.inherit_faction = true
								}},
							}
						}},
						Flag<FlagID::ActionOnRespawn>
						{{
							.actions =
							{
								Action<ActionID::TeleportToPlayer>{{}}
							}
						}}
					},
					.faction = Faction::PlayerFriend,
					.base_stats =
					{
						.movement_speed = 0.001f,
						.damage = default_base_damage * 60.0f,
						.max_health = 0.0001f,
						.current_health = 0.0f
					},
					.skin = ActorSkin::PlayerClassic_DefaultFireball,
					.animation = game::play_animation(AnimationID::PlayerClassic_DefaultFireball_Idle),
					.name = "Akhara's Default Orb"
				};
			break;
			case ActorType::FireSmoke:
				return
				{
					.type = ActorType::FireSmoke,
					.flags_new = 
					{
						Flag<FlagID::InvisibleWhileDead>{},
						Flag<FlagID::DieOnAnimationFinish>{},
						Flag<FlagID::Stealth>{},
						Flag<FlagID::Unhittable>{}
					},
					.skin = ActorSkin::FireSmoke,
					.name = "Fire Smoke Effect"
				};
			break;
			case ActorType::FireExplosion:
				return
				{
					.type = ActorType::FireExplosion,
					.flags_new =
					{
						Flag<FlagID::InvisibleWhileDead>{},
						Flag<FlagID::CustomScale>{{.scale = {1.5f, 1.5f}}},
						Flag<FlagID::HazardousIf>
						{{
							.predicate = [](const Actor& self, const Actor& actor)->bool
							{
								return self.is_enemy_of(actor);
							}
						}},
						Flag<FlagID::Stealth>{},
						Flag<FlagID::Unhittable>{},
						Flag<FlagID::DieOnAnimationFinish>{}
					},
					.skin = ActorSkin::FireExplosion,
					.name = "Fire Explosion Hazard"
				};
			break;
			case ActorType::BloodSplatter:
				return
				{
					.type = ActorType::BloodSplatter,
					.flags_new = 
					{
						Flag<FlagID::Stealth>{},
						Flag<FlagID::Unhittable>{},
						Flag<FlagID::DieOnAnimationFinish>{}
					},
					.skin = ActorSkin::BloodSplatter,
					.name = "Blood Splatter Effect"
				};
			break;
			case ActorType::Nightmare:
				return
				{
					.type = ActorType::Nightmare,
					.flags_new =
					{
						Flag<FlagID::BlockingAnimations>{},
						Flag<FlagID::RespawnOnDeath>{},
						Flag<FlagID::Aggressive>{},
						Flag<FlagID::HazardousIf>
						{{
							.predicate = [](const Actor& self, const Actor& victim){return self.is_enemy_of(victim);}
						}},
						Flag<FlagID::SelfRecoil>{}
					},
					.faction = Faction::PureEnemy,
					.base_stats =
					{
						.movement_speed = 0.0014f,
						.damage = 1.0f,
						.max_health = 0.001f,
						.current_health = 0.001f
					},
					.skin = ActorSkin::Nightmare,
					.palette_colour = {64u, 32u, 8u},
					.name = "Nightmare Boss"
				};
			break;
			case ActorType::GhostZombie_Spawner:
				return
				{
					.type = ActorType::GhostZombie_Spawner,
					.flags_new =
					{
						Flag<FlagID::RespawnOnDeath>{},
						Flag<FlagID::Rot>{},
						Flag<FlagID::Stealth>{},
						Flag<FlagID::ActionOnDeath>
						{{
							.actions =
							{
								Action<ActionID::SpawnActor>
								{{
									.actor = ActorType::GhostZombie,

								}}
							}
						}},
						Flag<FlagID::ActionOnPlayerTouch>
						{{
							.actions =
							{
								Action<ActionID::RandomTeleport>{}
							}
						}},
					},
					.skin = ActorSkin::DebugOnlyVisible,
					.palette_colour = {255u, 64u, 255u},
					.name = "Spawner (Ghost Zombie)",
					.layer = default_layer + 1
				};
			break;
			case ActorType::Wall:
				return
				{
					.type = ActorType::Wall,
					.flags_new =
					{
						Flag<FlagID::Collide>
						{{
							.collision_blacklist =
							{
								ActorType::FireSmoke,
								ActorType::FireExplosion,
								ActorType::BloodSplatter,
								ActorType::GhostZombie_Spawner,
							}
						}},
						Flag<FlagID::Invincible>{},
						Flag<FlagID::CustomScale>{{.scale = {0.65f, 0.65f}}},
						Flag<FlagID::Stealth>{}
					},
					.faction = Faction::PureEnemy,
					.skin = ActorSkin::Material_Stone,
					.palette_colour = {255u, 0u, 0u},
					.name = "Wall",
					.density = 2.0f,
					.layer = 255
				};
			break;
			case ActorType::WallDestructible:
				return
				{
					.type = ActorType::WallDestructible,
					.flags_new =
					{
						Flag<FlagID::DoNotGarbageCollect>{},
						Flag<FlagID::Collide>
						{{
							.collision_blacklist =
							{
								ActorType::FireSmoke,
								ActorType::FireExplosion,
								ActorType::BloodSplatter,
								ActorType::GhostZombie_Spawner
							}
						}},
						Flag<FlagID::CustomScale>{{.scale = {0.65f, 0.65f}}},
						Flag<FlagID::Stealth>{}
					},
					.faction = Faction::PlayerEnemy,
					.base_stats =
					{
						.max_health = 1.0f,
						.current_health = 1.0f
					},
					.skin = ActorSkin::Material_Stone,
					.palette_colour = {127u, 0u, 0u},
					.name = "Destructible Wall",
					.density = 2.0f,
					.layer = 255
				};
			break;
			case ActorType::CollectablePowerup_Sprint:
				return
				{
					.type = ActorType::CollectablePowerup_Sprint,
					.flags_new =
					{
						Flag<FlagID::ActionOnPlayerTouch>
						{{
							.actions =
							{
								Action<ActionID::ApplyBuffToPlayers>
								{{
									.buff = BuffID::Sprint
								}}
							}
						}}
					},
					.skin = ActorSkin::Powerup_Sprint,
					.palette_colour = {255u, 255u, 0u},
					.name = "Sprint Powerup"
				};
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
		TZ_PROFZONE("Actor - Update", TZ_PROFCOL_GREEN);
		this->evaluate_buffs();
		this->entity.update();
		this->evaluate_animation();
		if(this->flags_new.has<FlagID::BlockingAnimations>())
		{
			if(!this->animation.get_info().loop && !this->animation.complete())
			{
				return;
			}
		}
		const bool is_left_clicked = tz::window().get_mouse_button_state().is_mouse_button_down(tz::MouseButton::Left) && !tz::dbgui::claims_mouse();
		if(!this->dead())
		{
			if(this->flags_new.has<FlagID::ClickToLaunch>() && is_left_clicked)
			{
				auto& flag = this->flags_new.get<FlagID::ClickToLaunch>()->data();
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
			if(this->flags_new.has<FlagID::MouseControlled>())
			{
				this->entity.set<ActionID::GotoMouse>();
			}
			if(this->flags_new.has<FlagID::KeyboardControlled>())
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
			if(this->flags_new.has<FlagID::RespawnOnClick>() && is_left_clicked)
			{
				this->respawn();
				return;
			}
		}
	}

	bool Actor::dead() const
	{
		return !this->flags_new.has<FlagID::Invincible>() && this->base_stats.current_health <= 0.0f;
	}

	void Actor::dbgui()
	{
		ImGui::Text("Flags New Count: %zu", this->flags_new.size());
		ImGui::Text("\"%s\" (HP: %.2f/%.2f, dead: %s)", this->name, this->base_stats.current_health, this->get_current_stats().max_health, this->dead() ? "true" : "false");
		ImGui::SameLine();
		if(ImGui::Button("Kill"))
		{
			this->base_stats.current_health = 0;
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

			ImGui::Text("Controlled: %s", this->flags_new.has<FlagID::MouseControlled>() ? "true" : "false");
			ImGui::SameLine();
			if(ImGui::Button("Toggle Controlled"))
			{
				if(this->flags_new.has<FlagID::MouseControlled>())
				{
					this->flags_new.remove<FlagID::MouseControlled>();
				}
				else
				{
					this->flags_new.add<FlagID::MouseControlled>();
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
		if(this->flags_new.has<FlagID::ActionOnRespawn>())
		{
			auto& flag = this->flags_new.get<FlagID::ActionOnRespawn>()->data();
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
		if(this->flags_new.has<FlagID::Player>() && actor.flags_new.has<FlagID::Player>())
		{
			return true;
		}
		// PlayerFriends cannot hurt players.
		if(this->faction == Faction::PlayerFriend && actor.flags_new.has<FlagID::Player>())
		{
			return true;
		}
		// Players cannot hurt PlayerFriends.
		if(this->flags_new.has<FlagID::Player>() && actor.faction == Faction::PlayerFriend)
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
		if(this->flags_new.has<FlagID::Player>() && actor.faction == Faction::PlayerEnemy)
		{
			return true;
		}
		// PlayerEnemy faction members see Players as enemies.
		if(this->faction == Faction::PlayerEnemy && actor.flags_new.has<FlagID::Player>())
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
		TZ_PROFZONE("Actor - Buff Evaluation", TZ_PROFCOL_GREEN);
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
		TZ_PROFZONE("Actor - Animation Evaluation", TZ_PROFCOL_GREEN);
		AnimationID ending_animation = AnimationID::Missing;
		switch(this->skin)
		{
			case ActorSkin::PlayerClassic_DefaultFireball:
				ending_animation = AnimationID::PlayerClassic_DefaultFireball_Idle;
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
			case ActorSkin::BloodSplatter:
				ending_animation = AnimationID::BloodSplatter;
			break;
			case ActorSkin::Powerup_Sprint:
				ending_animation = AnimationID::Powerup_Sprint;
			break;
			case ActorSkin::FireSmoke:
				ending_animation = AnimationID::PlayerClassic_FireSmoke;
			break;
			case ActorSkin::FireExplosion:
				ending_animation = AnimationID::PlayerClassic_FireExplosion;
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
