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
					.flags = {ActorFlag::KeyboardControlled},
					.base_movement = 0.1f,
					.skin = ActorSkin::PlayerClassic,
					.animation = game::play_animation(AnimationID::PlayerClassic_Idle)
				};
			break;
		}
		return Actor::null();
	}

	void Actor::update()
	{
		this->actions = {};
		if(this->flags.contains(ActorFlag::KeyboardControlled))
		{
			const auto& kb = tz::window().get_keyboard_state();
			bool should_move = false;
			if(kb.is_key_down(tz::KeyCode::W))
			{
				should_move = true;
				switch(this->skin)
				{
					case ActorSkin::PlayerClassic:
						this->assign_animation(AnimationID::PlayerClassic_MoveUp);
					break;
				}
			}
			if(kb.is_key_down(tz::KeyCode::A))
			{
				should_move = true;
				switch(this->skin)
				{
					case ActorSkin::PlayerClassic:
						this->assign_animation(AnimationID::PlayerClassic_MoveSide);
					break;
				}
			}
			if(kb.is_key_down(tz::KeyCode::S))
			{
				should_move = true;
				switch(this->skin)
				{
					case ActorSkin::PlayerClassic:
						this->assign_animation(AnimationID::PlayerClassic_MoveDown);
					break;
				}
			}
			if(kb.is_key_down(tz::KeyCode::D))
			{
				should_move = true;
				switch(this->skin)
				{
					case ActorSkin::PlayerClassic:
						this->assign_animation(AnimationID::PlayerClassic_MoveSide);
					break;
				}
				this->actions |= ActorAction::HorizontalFlip;
			}

			if(!should_move)
			{
				switch(this->skin)
				{
					case ActorSkin::PlayerClassic:
						this->assign_animation(AnimationID::PlayerClassic_Idle);
					break;
				}
			}
		}
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
