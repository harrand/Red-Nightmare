#include "actor.hpp"
#include "tz/core/tz.hpp"

namespace game
{
	Actor create_actor(ActorType type)
	{
		switch(type)
		{
			case ActorType::PlayerClassic:
				return
				{
					.flags = {ActorFlag::Player, ActorFlag::KeyboardControlled},
					.base_movement = 0.001f,
					.skin = ActorSkin::PlayerClassic,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle)
				};
			break;
			case ActorType::PlayerClassic_TestEvil:
				return
				{
					.flags = {ActorFlag::HostileGhost},
					.base_movement = 0.0005f,
					.skin = ActorSkin::PlayerClassic,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle)
				};
			break;
		}
		return Actor::null();
	}

	void Actor::update()
	{
		this->refresh_actions();
		if(this->flags.contains(ActorFlag::HostileGhost))
		{
			// If it wants to chase the player the whole time, let it!
			this->actions |= ActorAction::ChasePlayer;
		}
		AnimationID ending_animation = AnimationID::Missing;
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
		switch(this->skin)
		{
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

	void Actor::refresh_actions()
	{
		ActorActions preserved_actions;
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
