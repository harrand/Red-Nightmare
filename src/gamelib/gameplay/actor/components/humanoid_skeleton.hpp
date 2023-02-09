// a humanoid skeleton has a certain number of animations it can be doing at once.
enum class humanoid_skeleton_animation
{
	idle,
	move_side,
	move_up,
	move_down,
	_count
};

template<>
struct actor_component_params<actor_component_id::humanoid_skeleton>
{
	// list of all poses.
	std::array<actor_component_params<actor_component_id::animation>, static_cast<int>(humanoid_skeleton_animation::_count)> poses;
	bool move_animation_affected_by_speed = true;
	// implementation detail. if we were moving left last update and we still are, we dont want to re-assign the animation as it would reset the animation timer and stick in the same frame forever.
	move_direction_t impl_movedir = 4;
};

template<>
inline void actor_component_update<actor_component_id::humanoid_skeleton>
(
	actor_component<actor_component_id::humanoid_skeleton>& component,
	float dt,
	actor& actor
)
{
	// check that our dependent components exist.
	tz::assert(actor.entity.has_component<actor_component_id::animation>(), "`humanoid_skeleton` component detected on an entity, but the entity doesn't have an `animation` component to manipulate!");
	tz::assert(actor.entity.has_component<actor_component_id::motion>(), "`humanoid_skeleton` component detected on an entity, but the entity doesn't have a `motion` component to manipulate!");
	auto& animation = *actor.entity.get_component<actor_component_id::animation>();
	auto& motion = *actor.entity.get_component<actor_component_id::motion>();
	// set our animation depending on what the motion says our move direction currently is.

	move_direction_t dir = motion.data().direction;
	auto get_pose = [&component](humanoid_skeleton_animation anim)
	{
		return component.data().poses[static_cast<int>(anim)];
	};

	if(component.data().impl_movedir == dir)
	{
		return;
	}
	component.data().impl_movedir = dir;

	if(dir & move_direction::right)
	{
		animation.data() = get_pose(humanoid_skeleton_animation::move_side);
		if(component.data().move_animation_affected_by_speed)
		{
			animation.data().animation_speed = motion.data().speed;
		}
		actor.transform.local_scale[0] = -std::abs(actor.transform.local_scale[0]);
	}
	else if(dir & move_direction::left)
	{
		animation.data() = get_pose(humanoid_skeleton_animation::move_side);
		if(component.data().move_animation_affected_by_speed)
		{
			animation.data().animation_speed = motion.data().speed;
		}
		actor.transform.local_scale[0] = std::abs(actor.transform.local_scale[0]);
	}
	else if(dir & move_direction::up)
	{
		animation.data() = get_pose(humanoid_skeleton_animation::move_up);
		if(component.data().move_animation_affected_by_speed)
		{
			animation.data().animation_speed = motion.data().speed;
		}
	}
	else if(dir & move_direction::down)
	{
		animation.data() = get_pose(humanoid_skeleton_animation::move_down);
		if(component.data().move_animation_affected_by_speed)
		{
			animation.data().animation_speed = motion.data().speed;
		}
	}
	else
	{
		animation.data() = get_pose(humanoid_skeleton_animation::idle);
	}
}

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::humanoid_skeleton>& component)
{
	move_direction_t dir = component.data().impl_movedir;

	const char* pose = "<unknown>";
	if(dir & move_direction::right)
	{
		pose = "move right";
	}
	else if(dir & move_direction::left)
	{
		pose = "move left";
	}
	else if(dir & move_direction::up)
	{
		pose = "move up";
	}
	else if(dir & move_direction::down)
	{
		pose = "move down";
	}
	else
	{
		pose = "idle";
	}
	ImGui::Text("pose: %s", pose);
}
