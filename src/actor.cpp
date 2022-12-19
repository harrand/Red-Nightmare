#include "actor.hpp"
#include "util.hpp"
#include "tz/core/tz.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "hdk/profile.hpp"

using namespace tz::literals;

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
					.flags =
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
						}},
						Flag<FlagID::ActionOnDeath>
						{{
							.actions =
							{
								Action<ActionID::DelayedAction>
								{{
									.delay_millis = 2000.0f,
									.actions =
									{
										Action<ActionID::Respawn>{}
									}
								}}
							}
						}},
						Flag<FlagID::ActionOnClick>
						{{
							.action_map =
							{
								{tz::MouseButton::Left,
									{
										.actions =
										{
											Action<ActionID::Cast>
											{{
												.cast_time_millis = 500.0f,
												.actions =
												{
													Action<ActionID::SpawnActor>
													{{
														.actor = ActorType::ChaoticFireball,
														.inherit_faction = true,
														.actions =
														{
															Action<ActionID::LaunchToMouse>
															{{
																.speed_multiplier = 8.0f
															}}
														}
													}}
												},
												.player_cancel_cast_escape = true
											}}
										},
										.icd = 550_ms
									}
								},
								{tz::MouseButton::Right,
									{
										.actions = 
										{
											Action<ActionID::Cast>
											{{
												.cast_time_millis = 1500.0f,
												.actions =
												{
													Action<ActionID::Despawn>{},
													Action<ActionID::SpawnActor>
													{{
														.actor = ActorType::ChaoticFireball,
														.inherit_faction = true,
														.actions =
														{
															Action<ActionID::LaunchToMouse>
															{{
																.speed_multiplier = 8.0f
															}},
															Action<ActionID::ApplyFlag>
															{{
																.flags =
																{
																	Flag<FlagID::CustomScale>
																	{{
																		.scale = {1.0f, 1.0f}
																	}},
																	Flag<FlagID::Player>{},
																	Flag<FlagID::ActionOnRepeat>
																	{{
																		.period = 250.0f,
																		.current_time = 500.0f,
																		.predicate = [](const Actor&){return true;},
																		.actions =
																		{
																			Action<ActionID::SpawnActor>
																			{{
																				.actor = ActorType::FireExplosion,
																				.inherit_faction = true
																			}},
																		}
																	}},
																	Flag<FlagID::ActionOnDeath>
																	{{
																		.actions =
																		{
																			Action<ActionID::SpawnActor>
																			{{
																				.actor = ActorType::FireExplosion,
																				.inherit_faction = true
																			}},
																			Action<ActionID::RespawnAs>
																			{{
																				.actor = ActorType::PlayerClassic
																			}}
																		}
																	}}
																}
															}}
														}
													}}
												},
												.player_cancel_cast_escape = true
											}},
										},
										.icd = 500_ms
									}
								}
							}
						}}
					},
					.faction = Faction::PlayerFriend,
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
					.flags =
					{
						Flag<FlagID::Collide>
						{{
							.collision_filter =
							{
								ActorType::GhostZombie,
								ActorType::GhostBanshee
							}
						}},
						Flag<FlagID::ActionOnActorTouch>
						{{
							.predicate = [](const Actor& self, const Actor& actor)
							{
								return actor.type == ActorType::GhostBanshee_Spirit;
							},
							.actions =
							{
								Action<ActionID::Die>{}
							}
						}},
						Flag<FlagID::AggressiveIf>
						{{
							.predicate = [](const Actor& self, const Actor& victim){return self.is_enemy_of(victim);}
						}},
						Flag<FlagID::HazardousIf>
						{{
							.predicate = [](const Actor& self, const Actor& victim){return self.is_enemy_of(victim);}
						}},
						Flag<FlagID::RandomSkin>
						{{
							.skins =
							{
								ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie,ActorSkin::GhostZombie,ActorSkin::GhostZombie, ActorSkin::GhostZombie,ActorSkin::GhostZombie,ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, ActorSkin::GhostZombie, 
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
					.flags =
					{
						Flag<FlagID::Collide>
						{{
							.collision_filter =
							{
								ActorType::GhostZombie,
								ActorType::GhostBanshee
							}
						}},
						Flag<FlagID::AggressiveIf>
						{{
							.predicate = [](const Actor& self, const Actor& victim){return self.is_enemy_of(victim);}
						}},
						Flag<FlagID::HazardousIf>
						{{
							.predicate = [](const Actor& self, const Actor& victim){return self.is_enemy_of(victim);}
						}},
						Flag<FlagID::ActionOnHit>
						{{
							.hittee_actions =
							{
								Action<ActionID::ApplyBuff>
								{{
									.buff = BuffID::Chill
								}}
							}
						}},
						Flag<FlagID::ActionOnDeath>
						{{
							.actions =
							{
								Action<ActionID::RespawnAs>
								{{
									.actor = ActorType::GhostBanshee_Spirit
								}}
							}
						}},
						Flag<FlagID::ActionOnRepeat>
						{{
							.period = 3000.0f,
							.current_time = 10000.0f,
							.actions =
							{
								Action<ActionID::Cast>
								{{
									.cast_time_millis = 800.0f,
									.actions =
									{
										Action<ActionID::SpawnActor>
										{{
											.actor = ActorType::ChaoticFireball,
											.inherit_faction = true,
											.actions =
											{
												Action<ActionID::LaunchToPlayer>
												{{
													.speed_multiplier = 8.0f
												}}
											}
										}}
									},
									.cast_while_moving = true
								}},
							}
						}}
					},
					.faction = Faction::PlayerEnemy,
					.base_stats =
					{
						.movement_speed = 0.0004f,
						.max_health = 0.01f,
						.current_health = 0.01f
					},
					.skin = ActorSkin::GhostBanshee,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle),
					.palette_colour = {128u, 255u, 255u},
					.name = "Ghost Banshee"
				};
			break;
			case ActorType::GhostBanshee_Spirit:
				return
				{
					.type = ActorType::GhostBanshee_Spirit,
					.flags =
					{
						Flag<FlagID::Collide>
						{{
							.collision_filter =
							{
								ActorType::GhostZombie,
								ActorType::GhostBanshee
							}
						}},
						Flag<FlagID::Rot>{},
						Flag<FlagID::AggressiveIf>
						{{
							.predicate = [](const Actor& self, const Actor& victim){return victim.type == ActorType::GhostZombie;}
						}},
						Flag<FlagID::ActionOnActorTouch>
						{{
							.predicate = [](const Actor& self, const Actor& actor)
							{
								return actor.type == ActorType::GhostZombie && self.is_ally_of(actor);
							},
							.actions =
							{
								Action<ActionID::RespawnAs>
								{{
									.actor = ActorType::GhostBanshee
								}}
							}
						}}
					},
					.faction = Faction::PureFriend,
					.base_stats =
					{
						.movement_speed = 0.004f,
						.max_health = 8.0f,
						.current_health = 8.0f
					},
					.skin = ActorSkin::GhostBanshee,
					.name = "Ghost Banshee Spirit"
				};
			break;
			case ActorType::ChaoticFireball:
				return
				{
					.type = ActorType::ChaoticFireball,
					.flags =
					{
						Flag<FlagID::Projectile>{},
						Flag<FlagID::CustomGarbageCollectPeriod>
						{{
							.delay_millis = 500ull
						}},
						Flag<FlagID::InvisibleWhileDead>{},
						Flag<FlagID::CustomScale>{{.scale = {0.65f, 0.65f}}},
						Flag<FlagID::HazardousIf>
						{{
							.predicate = [](const Actor& self, const Actor& actor)->bool
							{
								return self.is_enemy_of(actor);
							}
						}},
						Flag<FlagID::ActionOnOOL>
						{{
							.actions =
							{
								Action<ActionID::Die>{}
							}
						}},
						Flag<FlagID::Stealth>{},
						Flag<FlagID::SelfRecoil>{},
						Flag<FlagID::Unhittable>{},
						Flag<FlagID::ActionOnActorTouch>
						{{
							.predicate = [](const Actor& self, const Actor& actor)
							{
								return self.type == actor.type && self.is_enemy_of(actor);
							},
							.actions =
							{
								Action<ActionID::Die>{}
							}
						}},
						Flag<FlagID::ActionOnDeath>
						{{
							.actions =
							{
								Action<ActionID::SpawnActor>
								{{
									.actor = ActorType::FireSmoke,
								}},
							}
						}}
					},
					.faction = Faction::PlayerEnemy,
					.base_stats =
					{
						.movement_speed = 0.001f,
						.damage = default_base_damage * 60.0f * 5.0f,
						.max_health = 0.0001f,
						.current_health = 0.0001f
					},
					.skin = ActorSkin::PlayerClassic_DefaultFireball,
					.animation = game::play_animation(AnimationID::PlayerClassic_DefaultFireball_Idle),
					.name = "Fireball"
				};
			break;
			case ActorType::FireSmoke:
				return
				{
					.type = ActorType::FireSmoke,
					.flags = 
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
					.flags =
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
					.flags = 
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
					.flags =
					{
						Flag<FlagID::BlockingAnimations>{},
						Flag<FlagID::RespawnOnDeath>{},
						Flag<FlagID::AggressiveIf>
						{{
							.predicate = [](const Actor& self, const Actor& victim){return self.is_enemy_of(victim);}
						}},
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
					.flags =
					{
						Flag<FlagID::Stealth>{},
						Flag<FlagID::InvisibleWhileDead>{},
						Flag<FlagID::ActionOnRepeat>
						{{
							.period = 10000.0f,
							.predicate = [](const Actor& self){return !self.dead();},
							.actions =
							{
								Action<ActionID::SpawnActor>
								{{
									.actor = ActorType::GhostZombie,

								}}
							}
						}},
					},
					.faction = Faction::PlayerEnemy,
					.skin = ActorSkin::DebugOnlyVisible,
					.palette_colour = {255u, 64u, 255u},
					.name = "Spawner (Ghost Zombie)",
					.layer = default_layer + 1
				};
			break;
			case ActorType::Fireball_Spawner:
				return
				{
					.type = ActorType::Fireball_Spawner,
					.flags =
					{
						Flag<FlagID::Stealth>{},
						Flag<FlagID::InvisibleWhileDead>{},
						Flag<FlagID::ActionOnRepeat>
						{{
							.period = 5000.0f,
							.predicate = [](const Actor& self){return !self.dead();},
							.actions =
							{
								Action<ActionID::SpawnActor>
								{{
									.actor = ActorType::ChaoticFireball,
									.inherit_faction = true,
									.actions =
									{
										Action<ActionID::LaunchToPlayer>
										{{
											.speed_multiplier = 8.0f
										}}
									}
								}}
							}
						}},
					},
					.faction = Faction::PlayerEnemy,
					.skin = ActorSkin::DebugOnlyVisible,
					.palette_colour = {255u, 32u, 255u},
					.name = "Spawner (Fireball)",
					.layer = default_layer + 1
				};
			break;
			case ActorType::Wall:
				return
				{
					.type = ActorType::Wall,
					.flags =
					{
						Flag<FlagID::Collide>
						{{
							.collision_blacklist =
							{
								ActorType::FireSmoke,
								ActorType::FireExplosion,
								ActorType::BloodSplatter,
								ActorType::GhostZombie_Spawner,
								ActorType::GhostBanshee_Spirit
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
			case ActorType::InvisibleWall:
				return
				{
					.type = ActorType::InvisibleWall,
					.flags =
					{
						Flag<FlagID::Collide>
						{{
							.collision_blacklist =
							{
								ActorType::FireSmoke,
								ActorType::FireExplosion,
								ActorType::BloodSplatter,
								ActorType::GhostZombie_Spawner,
								ActorType::GhostBanshee_Spirit
							},
						}},
						Flag<FlagID::Invincible>{},
						Flag<FlagID::CustomScale>{{.scale = {0.65f, 0.65f}}},
						Flag<FlagID::Stealth>{}
					},
					.faction = Faction::PureEnemy,
					.skin = ActorSkin::Invisible,
					.palette_colour = {128u, 128u, 128u},
					.name = "Invisible Wall",
					.density = 2.0f,
					.layer = 255
				};
			break;
			case ActorType::InvisibleWallProjectilePass:
				return
				{
					.type = ActorType::InvisibleWallProjectilePass,
					.flags =
					{
						Flag<FlagID::Collide>
						{{
							.collision_blacklist =
							{
								ActorType::FireSmoke,
								ActorType::FireExplosion,
								ActorType::BloodSplatter,
								ActorType::GhostZombie_Spawner,
								ActorType::GhostBanshee_Spirit
							},
							.blacklist_predicate = [](const Actor& a){return a.flags.has<FlagID::Projectile>();}
						}},
						Flag<FlagID::Invincible>{},
						Flag<FlagID::Unhittable>{},
						Flag<FlagID::CustomScale>{{.scale = {0.65f, 0.65f}}},
						Flag<FlagID::Stealth>{}
					},
					.faction = Faction::PureEnemy,
					.skin = ActorSkin::Invisible,
					.palette_colour = {255u, 255u, 255u},
					.name = "Invisible Wall (Projectiles Pass)",
					.density = 2.0f,
					.layer = 255
				};
			break;
			case ActorType::WallDestructible:
				return
				{
					.type = ActorType::WallDestructible,
					.flags =
					{
						Flag<FlagID::DoNotGarbageCollect>{},
						Flag<FlagID::Collide>
						{{
							.collision_blacklist =
							{
								ActorType::FireSmoke,
								ActorType::FireExplosion,
								ActorType::BloodSplatter,
								ActorType::GhostZombie_Spawner,
								ActorType::GhostBanshee_Spirit
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
					.flags =
					{
						Flag<FlagID::ActionOnActorTouch>
						{{
							.predicate = [](const Actor& self, const Actor& actor)
							{
								return actor.type == ActorType::PlayerClassic;
							},
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
		HDK_PROFZONE("Actor - Update", 0xFF00AA00);
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
		const bool is_left_clicked = tz::window().get_mouse_button_state().is_mouse_button_down(tz::MouseButton::Left) && !tz::dbgui::claims_mouse();
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
					if(tz::window().get_mouse_button_state().is_mouse_button_down(button) && data.icd.done())
					{
						data.actions.copy_components(this->entity);
						data.icd.reset();
					}
				}
			}
			if(this->flags.has<FlagID::KeyboardControlled>())
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
		ImGui::Text("Flags New Count: %zu", this->flags.size());
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
		HDK_PROFZONE("Actor - Buff Evaluation", 0xFF00AA00);
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
		HDK_PROFZONE("Actor - Animation Evaluation", 0xFF00AA00);
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
					ending_animation = AnimationID::PlayerClassic_LowPoly_Death;
				}
				if(this->entity.has<ActionID::Cast>())
				{
					const auto& action = this->entity.get<ActionID::Cast>()->data();
					const Animation end_cast_anim = game::play_animation(AnimationID::PlayerClassic_LowPoly_EndCast);
					const int end_cast_anim_time_millis = (static_cast<float>(end_cast_anim.get_info().frames.size()) / end_cast_anim.get_info().fps) * 1000.0f;
					if(action.cast_time_millis <= end_cast_anim_time_millis)
					{
						ending_animation = AnimationID::PlayerClassic_LowPoly_EndCast;
					}
					else
					{
						ending_animation = AnimationID::PlayerClassic_LowPoly_Cast;
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
				#if HDK_DEBUG
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
