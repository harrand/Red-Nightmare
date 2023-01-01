#include "actor.hpp"
#include "util.hpp"
#include "tz/core/tz.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "hdk/profile.hpp"

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
				return
				{
					.type = ActorType::PlayerAkhara,
					.flags =
					{
						Flag<FlagID::DoNotGarbageCollect>{},
						Flag<FlagID::StatTracked>{},
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
														.actor = ActorType::FrozenOrb,
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
																				.actor = ActorType::PlayerAkhara
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
					.skin = ActorSkin::PlayerAkhara_LowPoly,
					.animation = game::play_animation(AnimationID::PlayerAkhara_Idle),
					.name = "Player Akhara"
				};
			break;
			case ActorType::GhostZombie:
				return
				{
					.type = ActorType::GhostZombie,
					.flags =
					{
						Flag<FlagID::StatTracked>{},
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
							.predicate = [](const Actor& self, const Actor& victim){return self.is_enemy_of(victim) && !victim.flags.has<FlagID::Obstacle>() && !victim.dead();}
						}},
						Flag<FlagID::HazardousIf>
						{{
							.predicate = [](const Actor& self, const Actor& victim){return self.is_enemy_of(victim) && !victim.flags.has<FlagID::Obstacle>();}
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
					.animation = game::play_animation(AnimationID::PlayerAkhara_Idle),
					.palette_colour = {0u, 255u, 0u},
					.name = "Ghost Zombie",
					.density = 0.1f
				};
			break;
			case ActorType::GhostBanshee:
				return
				{
					.type = ActorType::GhostBanshee,
					.flags =
					{
						Flag<FlagID::Projectile>{},
						Flag<FlagID::StatTracked>{},
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
							.predicate = [](const Actor& self, const Actor& victim){return self.is_enemy_of(victim) && !victim.dead();}
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
											.actor = ActorType::Frostbolt,
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
					.animation = game::play_animation(AnimationID::PlayerAkhara_Idle),
					.palette_colour = {128u, 255u, 255u},
					.name = "Ghost Banshee",
					.density = 0.1f
				};
			break;
			case ActorType::GhostBanshee_Spirit:
				return
				{
					.type = ActorType::GhostBanshee_Spirit,
					.flags =
					{
						Flag<FlagID::StatTracked>{},
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
							.predicate = [](const Actor& self, const Actor& victim){return victim.flags.has<FlagID::GoesDownALevel>();}
						}},
						Flag<FlagID::ActionOnActorTouch>
						{{
							.predicate = [](const Actor& self, const Actor& actor)
							{
								return actor.flags.has<FlagID::GoesDownALevel>();
							},
							.actions =
							{
								Action<ActionID::Despawn>{}
							},
							.touchee_actions =
							{
								Action<ActionID::ApplyFlag>
								{{
									.flags =
									{
										Flag<FlagID::Invincible>{}
									}
								}}
							},
							.allow_dead = true
						}}
					},
					.faction = Faction::PureFriend,
					.base_stats =
					{
						.movement_speed = 0.004f,
						.max_health = 80.0f,
						.current_health = 80.0f
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
						Flag<FlagID::Light>
						{{
							.light =
							{
								.colour = {1.0f, 0.1f, 0.02f},
								.power = 0.005f
							}
						}},
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
								return actor.flags.has<FlagID::Projectile>() && self.is_enemy_of(actor);
							},
							.actions =
							{
								Action<ActionID::Die>{}
							},
							.touchee_actions =
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
					.skin = ActorSkin::Invisible,
					.animation = game::play_animation(AnimationID::PlayerAkhara_DefaultFireball_Idle),
					.name = "Fireball"
				};
			break;
			case ActorType::Frostbolt:
				return
				{
					.type = ActorType::Frostbolt,
					.flags =
					{
						Flag<FlagID::Light>
						{{
							.light =
							{
								.colour = {0.01f, 0.1f, 1.0f},
								.power = 0.005f
							}
						}},
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
						Flag<FlagID::ActionOnHit>
						{{
							.hittee_actions
							{
								Action<ActionID::ApplyBuff>
								{{
									.buff = BuffID::Chill
								}},
							},
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
					.skin = ActorSkin::Invisible,
					.animation = game::play_animation(AnimationID::PlayerAkhara_DefaultFireball_Idle),
					.name = "Frostbolt"
				};
			break;
			case ActorType::FrozenOrb:
				return
				{
					.type = ActorType::FrozenOrb,
					.flags =
					{
						Flag<FlagID::Light>
						{{
							.light =
							{
								.colour = {0.01f, 0.1f, 1.0f},
								.power = 0.015f
							}
						}},
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
						Flag<FlagID::ActionOnHit>
						{{
							.hittee_actions
							{
								Action<ActionID::ApplyBuff>
								{{
									.buff = BuffID::Chill
								}},
							},
						}},
						Flag<FlagID::ActionOnRepeat>
						{{
							.period = 250.0f,
							.current_time = 250.0f,
							.predicate = [](const Actor& a){return !a.dead();},
							.actions =
							{
								Action<ActionID::SpawnActor>
								{{
									.actor = ActorType::Frostbolt,
									.inherit_faction = true,
									.actions =
									{
										Action<ActionID::LaunchRandomDirection>
										{{
											.speed_multiplier = 8.0f
										}}
									}
								}}
							}
						}}
					},
					.faction = Faction::PlayerEnemy,
					.base_stats =
					{
						.movement_speed = 0.0005f,
						.damage = default_base_damage * 60.0f * 5.0f,
						.max_health = 0.1f,
						.current_health = 0.1f
					},
					.skin = ActorSkin::Invisible,
					.animation = game::play_animation(AnimationID::PlayerAkhara_DefaultFireball_Idle),
					.name = "Frostbolt"
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
						Flag<FlagID::Light>
						{{
							.light =
							{
								.colour = {1.0f, 0.1f, 0.02f},
								.power = 0.00025f
							}
						}},
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
						Flag<FlagID::StatTracked>{},
						Flag<FlagID::BlockingAnimations>{},
						Flag<FlagID::RespawnOnDeath>{},
						Flag<FlagID::AggressiveIf>
						{{
							.predicate = [](const Actor& self, const Actor& victim){return self.is_enemy_of(victim) && !victim.dead();}
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
						Flag<FlagID::Light>
						{{
							.light =
							{
								.colour = {-1.0f, -0.01f, -1.0f},
								.power = 0.1f
							},
							.power_variance = 0.01f,
							.variance_rate = 0.08f,
							.min_variance_pct = 0.05f,
							.power_scale_with_health_pct = true
						}},
						Flag<FlagID::ActionOnRepeat>
						{{
							.period = 25000.0f,
							.predicate = [](const Actor& self){return !self.dead();},
							.actions =
							{
								Action<ActionID::SpawnActor>
								{{
									.actor = ActorType::GhostZombie,
									.inherit_faction = true

								}}
							}
						}},
					},
					.faction = Faction::PlayerEnemy,
					.skin = ActorSkin::Invisible,
					.palette_colour = {255u, 64u, 255u},
					.name = "Spawner Ghost Zombie",
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
						Flag<FlagID::Light>
						{{
							.light =
							{
								.colour = {-0.01f, -1.0f, -1.0f},
								.power = 0.1f
							},
							.power_variance = 0.01f,
							.variance_rate = 0.4f,
							.min_variance_pct = 0.05f,
							.power_scale_with_health_pct = true
						}},
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
					.skin = ActorSkin::Invisible,
					.palette_colour = {255u, 32u, 255u},
					.name = "Spawner Fireball",
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
					.layer = default_layer + 2
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
					.layer = default_layer + 2
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
					.name = "Invisible Wall Projectiles Pass",
					.layer = default_layer + 2
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
								ActorType::GhostBanshee_Spirit,
							}
						}},
						Flag<FlagID::CustomScale>{{.scale = {0.65f, 0.65f}}},
						Flag<FlagID::Stealth>{}
					},
					.faction = Faction::PureEnemy,
					.base_stats =
					{
						.max_health = 1.0f,
						.current_health = 1.0f
					},
					.skin = ActorSkin::Material_Stone,
					.palette_colour = {127u, 0u, 0u},
					.name = "Destructible Wall",
					.layer = default_layer + 2
				};
			break;
			case ActorType::Scenery_Gravestone:
				return
				{
					.type = ActorType::Scenery_Gravestone,
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
						Flag<FlagID::Stealth>{},
						Flag<FlagID::Obstacle>{},
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
						Flag<FlagID::RandomSkin>
						{{
							.skins =
							{
								ActorSkin::Scenery_Gravestone_0,
								ActorSkin::Scenery_Gravestone_1
							}
						}}
					},
					.faction = Faction::PureEnemy,
					.base_stats =
					{
						.max_health = 1.0f,
						.current_health = 1.0f
					},
					.skin = ActorSkin::Scenery_Gravestone_0,
					.palette_colour = {48u, 48u, 48u},
					.name = "Gravestone",
					.layer = default_layer + 2
				};
			break;
			case ActorType::Interactable_Torch:
				return
				{
					.type = ActorType::Interactable_Torch,
					.flags =
					{
						Flag<FlagID::Invincible>{},
						Flag<FlagID::CustomScale>{{.scale = {0.65f, 0.65f}}},
						Flag<FlagID::Stealth>{},
						Flag<FlagID::Light>
						{{
							.light =
							{
								.colour = {1.0f, 0.1f, 0.02f},
								.power = 0.001f
							},
							.offset = {-0.095f, 0.035f},
							.power_variance = 0.002f,
							.variance_rate = 15.0f
						}}
					},
					.faction = Faction::PureFriend,
					.skin = ActorSkin::Interactable_Torch,
					.palette_colour = {64u, 4u, 0u},
					.name = "Torch",
				};
			break;
			case ActorType::Interactable_Stone_Stairs_Down_NX:
				return
				{
					.type = ActorType::Interactable_Stone_Stairs_Down_NX,
					.flags =
					{
						Flag<FlagID::GoesDownALevel>{},
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
							},
							.blacklist_predicate = [](const Actor& a){return a.flags.has<FlagID::Player>();}
						}},
						Flag<FlagID::Unhittable>{},
						Flag<FlagID::CustomScale>{{.scale = {0.65f, 0.65f}}},
						Flag<FlagID::ActionOnActorTouch>
						{{
							.predicate = [](const Actor& self, const Actor& other){return other.flags.has<FlagID::Player>() && !other.flags.has<FlagID::SuppressedControl>() && !self.dead();},
							.touchee_actions =
							{
								Action<ActionID::ApplyFlag>
								{{
									.flags =
									{
										Flag<FlagID::SuppressedControl>{}
									}
								}},
								Action<ActionID::MoveRelative>
								{{
									.displacement = hdk::vec2{-1.0f, 0.0f},
									.timeout = 1000.0f
								}},
								Action<ActionID::DelayedAction>
								{{
									.delay_millis = level_transition_length,
									.actions =
									{
										Action<ActionID::NextLevel>{}
									}
								}}
							}
						}}
					},
					.faction = Faction::PureFriend,
					.base_stats =
					{
						.current_health = 0
					},
					.skin = ActorSkin::Interactable_Stone_Stairs_Down_NX,
					.palette_colour = {200u, 201u, 0u},
					.name = "Stone Stairs (Down, NX)"
				};
			break;
			case ActorType::Interactable_Stone_Stairs_Up_NX:
				return
				{
					.type = ActorType::Interactable_Stone_Stairs_Up_NX,
					.flags =
					{
						Flag<FlagID::GoesUpALevel>{},
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
							},
							.blacklist_predicate = [](const Actor& a){return a.flags.has<FlagID::Player>();}
						}},
						Flag<FlagID::Invincible>{},
						Flag<FlagID::Unhittable>{},
						Flag<FlagID::CustomScale>{{.scale = {0.65f, 0.65f}}},
						Flag<FlagID::ActionOnActorTouch>
						{{
							.predicate = [](const Actor& self, const Actor& other){return other.flags.has<FlagID::Player>() && !other.flags.has<FlagID::SuppressedControl>();},
							.touchee_actions =
							{
								Action<ActionID::ApplyFlag>
								{{
									.flags =
									{
										Flag<FlagID::SuppressedControl>{}
									}
								}},
								Action<ActionID::MoveRelative>
								{{
									.displacement = hdk::vec2{-1.0f, 0.0f},
									.timeout = 1000.0f
								}},
								Action<ActionID::DelayedAction>
								{{
									.delay_millis = level_transition_length,
									.actions =
									{
										Action<ActionID::PreviousLevel>{}
									}
								}}
							}
						}}
					},
					.faction = Faction::PureFriend,
					.skin = ActorSkin::Interactable_Stone_Stairs_Up_NX,
					.palette_colour = {200u, 201u, 255u},
					.name = "Stone Stairs (Up, NX)"
				};
			break;
			case ActorType::Interactable_Stone_Stairs_Down_PY:
				return
				{
					.type = ActorType::Interactable_Stone_Stairs_Down_PY,
					.flags =
					{
						Flag<FlagID::GoesDownALevel>{},
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
							},
							.blacklist_predicate = [](const Actor& a){return a.flags.has<FlagID::Player>();}
						}},
						Flag<FlagID::Unhittable>{},
						Flag<FlagID::CustomScale>{{.scale = {0.65f, 0.65f}}},
						Flag<FlagID::ActionOnActorTouch>
						{{
							.predicate = [](const Actor& self, const Actor& other){return other.flags.has<FlagID::Player>() && !other.flags.has<FlagID::SuppressedControl>() && !self.dead();},
							.touchee_actions =
							{
								Action<ActionID::ApplyFlag>
								{{
									.flags =
									{
										Flag<FlagID::SuppressedControl>{}
									}
								}},
								Action<ActionID::MoveRelative>
								{{
									.displacement = hdk::vec2{0.0f, 1.0f},
									.timeout = 1000.0f
								}},
								Action<ActionID::DelayedAction>
								{{
									.delay_millis = level_transition_length,
									.actions =
									{
										Action<ActionID::NextLevel>{}
									}
								}}
							}
						}}
					},
					.faction = Faction::PureFriend,
					.base_stats =
					{
						.current_health = 0
					},
					.skin = ActorSkin::Interactable_Stone_Stairs_Down_PY,
					.palette_colour = {200u, 200u, 0u},
					.name = "Stone Stairs (Down, PY)"
				};
			break;
			case ActorType::Interactable_Stone_Stairs_Up_PY:
				return
				{
					.type = ActorType::Interactable_Stone_Stairs_Up_PY,
					.flags =
					{
						Flag<FlagID::GoesUpALevel>{},
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
							},
							.blacklist_predicate = [](const Actor& a){return a.flags.has<FlagID::Player>();}
						}},
						Flag<FlagID::Invincible>{},
						Flag<FlagID::Unhittable>{},
						Flag<FlagID::CustomScale>{{.scale = {0.65f, 0.65f}}},
						Flag<FlagID::ActionOnActorTouch>
						{{
							.predicate = [](const Actor& self, const Actor& other){return other.flags.has<FlagID::Player>() && !other.flags.has<FlagID::SuppressedControl>();},
							.touchee_actions =
							{
								Action<ActionID::ApplyFlag>
								{{
									.flags =
									{
										Flag<FlagID::SuppressedControl>{}
									}
								}},
								Action<ActionID::MoveRelative>
								{{
									.displacement = hdk::vec2{0.0f, 1.0f},
									.timeout = 1000.0f
								}},
								Action<ActionID::DelayedAction>
								{{
									.delay_millis = level_transition_length,
									.actions =
									{
										Action<ActionID::PreviousLevel>{}
									}
								}}
							}
						}}
					},
					.faction = Faction::PureFriend,
					.skin = ActorSkin::Interactable_Stone_Stairs_Up_PY,
					.palette_colour = {200u, 200u, 255u},
					.name = "Stone Stairs (Up, PY)"
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
								return !actor.flags.has<FlagID::Projectile>();
							},
							.touchee_actions =
							{
								Action<ActionID::ApplyBuff>
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
			case ActorType::RoadSpeedBoost:
				return
				{
					.type = ActorType::RoadSpeedBoost,
					.flags =
					{
						Flag<FlagID::ActionOnActorTouch>
						{{
							.predicate = [](const Actor& self, const Actor& actor)
							{
								return actor.type == ActorType::PlayerAkhara;
							},
							.actions =
							{
								Action<ActionID::ApplyBuffToPlayers>
								{{
									.buff = BuffID::RoadSpeed
								}}
							}
						}}
					},
					.skin = ActorSkin::Invisible,
					.palette_colour = {64u, 0u, 255u},
					.name = "Road Speed Boost"
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
			if(this->flags.has<FlagID::KeyboardControlled>() && !this->flags.has<FlagID::SuppressedControl>())
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
