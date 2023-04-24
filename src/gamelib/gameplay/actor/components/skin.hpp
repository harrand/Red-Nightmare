struct skin_data
{
	tz::vec3 skin_colour = {1.0f, 0.875f, 0.679f};
	prefab::human_art::hair_style hair_style = prefab::human_art::hair_style::bald;
	tz::vec3 hair_colour = {0.8f, 0.0f, 0.0f};
	prefab::human_art::eye_type eye_type = prefab::human_art::eye_type::dots;
	tz::vec3 eye_colour = tz::vec3::zero();

	struct equipment_t
	{
		prefab::human_art::helm_type helm_type = prefab::human_art::helm_type::none;
		tz::vec3 helm_colour = tz::vec3::filled(1.0f);
		prefab::human_art::chest_type chest_type = prefab::human_art::chest_type::topless;
		tz::vec3 chest_colour = tz::vec3::filled(1.0f);
		prefab::human_art::feet_type feet_type = prefab::human_art::feet_type::bare;
		tz::vec3 feet_colour = tz::vec3::filled(1.0f);
	} equipment = {};
};

template<>
struct actor_component_params<actor_component_id::skin>
{
	skin_data data = {};
};

template<>
inline void actor_component_update<actor_component_id::skin>
(
	actor_component<actor_component_id::skin>& component,
	float dt,
	actor& actor
)
{
	if(!actor.entity.has_component<actor_component_id::humanoid_skeleton>())
	{
		actor.entity.add_component<actor_component_id::humanoid_skeleton>();
	}
	if(!actor.entity.has_component<actor_component_id::sprite>())
	{
		actor.entity.add_component<actor_component_id::sprite>
		({
			.textures =
	   		{
		   		// helm, hair, eyes, chest, feet, base
				{.id = image_id::undefined, .colour_tint = tz::vec3::zero()},
				{.id = image_id::undefined, .colour_tint = tz::vec3::zero()},
				{.id = image_id::undefined, .colour_tint = tz::vec3::zero()},
				{.id = image_id::undefined, .colour_tint = tz::vec3::zero()},
				{.id = image_id::undefined, .colour_tint = tz::vec3::zero()},
				{.id = image_id::undefined, .colour_tint = tz::vec3::zero()},
			}
		});
	}
	auto& sprite = *actor.entity.get_component<actor_component_id::sprite>();
	sprite.data().textures[0].colour_tint = component.data().data.equipment.helm_colour;
	sprite.data().textures[1].colour_tint = component.data().data.hair_colour;
	sprite.data().textures[2].colour_tint = component.data().data.eye_colour;
	sprite.data().textures[3].colour_tint = component.data().data.equipment.chest_colour;
	sprite.data().textures[4].colour_tint = component.data().data.equipment.feet_colour;
	sprite.data().textures[5].colour_tint = component.data().data.skin_colour;
	auto& skel = *actor.entity.get_component<actor_component_id::humanoid_skeleton>();
	auto get_hair_final = [](prefab::human_art::hair_style style, humanoid_skeleton_animation anim, prefab::human_art::helm_type helm)
	{
		if(prefab::human_art::helm_hides_hair(helm))
		{
			return prefab::human_art::get_hair(prefab::human_art::hair_style::bald, anim);
		}
		return prefab::human_art::get_hair(style, anim);
	};
	// HUMAN POSES
	// idle
	skel.data().poses[0] = actor_component_params<actor_component_id::animation>
	{
		.animations =
		{
			prefab::human_art::get_helm(component.data().data.equipment.helm_type, humanoid_skeleton_animation::idle),
			get_hair_final(component.data().data.hair_style, humanoid_skeleton_animation::idle, component.data().data.equipment.helm_type),
			prefab::human_art::get_eyes(component.data().data.eye_type, humanoid_skeleton_animation::idle),
			prefab::human_art::get_chest(component.data().data.equipment.chest_type, humanoid_skeleton_animation::idle),
			prefab::human_art::get_feet(component.data().data.equipment.feet_type, humanoid_skeleton_animation::idle),
			animation
			{{
				.frame_textures =
				{
					image_id::race_human_base_idle0,
					image_id::race_human_base_idle1
				},
				.fps = 2,
				.loop = true
			}}
		}
	};
	// move side
	skel.data().poses[1] = actor_component_params<actor_component_id::animation>
	{
		.animations =
		{
			prefab::human_art::get_helm(component.data().data.equipment.helm_type, humanoid_skeleton_animation::move_side),
			get_hair_final(component.data().data.hair_style, humanoid_skeleton_animation::move_side, component.data().data.equipment.helm_type),
			prefab::human_art::get_eyes(component.data().data.eye_type, humanoid_skeleton_animation::move_side),
			prefab::human_art::get_chest(component.data().data.equipment.chest_type, humanoid_skeleton_animation::move_side),
			prefab::human_art::get_feet(component.data().data.equipment.feet_type, humanoid_skeleton_animation::move_side),
			animation
			{{
				.frame_textures =
				{
					image_id::race_human_base_side0,
					image_id::race_human_base_side1,
					image_id::race_human_base_side2,
					image_id::race_human_base_side3
				},
				.fps = 6,
				.loop = true
			}}
		}
	};
	// move up
	skel.data().poses[2] = actor_component_params<actor_component_id::animation>
	{
		.animations =
		{
			prefab::human_art::get_helm(component.data().data.equipment.helm_type, humanoid_skeleton_animation::move_up),
			get_hair_final(component.data().data.hair_style, humanoid_skeleton_animation::move_up, component.data().data.equipment.helm_type),
			prefab::human_art::get_eyes(component.data().data.eye_type, humanoid_skeleton_animation::move_up),
			prefab::human_art::get_chest(component.data().data.equipment.chest_type, humanoid_skeleton_animation::move_up),
			prefab::human_art::get_feet(component.data().data.equipment.feet_type, humanoid_skeleton_animation::move_up),
			animation
			{{
				.frame_textures =
				{
					image_id::race_human_base_up0,
					image_id::race_human_base_up1,
					image_id::race_human_base_up2,
					image_id::race_human_base_up3
				},
				.fps = 6,
				.loop = true
			}}
		}
	};
	// move down
	skel.data().poses[3] = actor_component_params<actor_component_id::animation>
	{
		.animations =
		{
			prefab::human_art::get_helm(component.data().data.equipment.helm_type, humanoid_skeleton_animation::move_down),
			get_hair_final(component.data().data.hair_style, humanoid_skeleton_animation::move_down, component.data().data.equipment.helm_type),
			prefab::human_art::get_eyes(component.data().data.eye_type, humanoid_skeleton_animation::move_down),
			prefab::human_art::get_chest(component.data().data.equipment.chest_type, humanoid_skeleton_animation::move_down),
			prefab::human_art::get_feet(component.data().data.equipment.feet_type, humanoid_skeleton_animation::move_down),
			animation
			{{
				.frame_textures =
				{
					image_id::race_human_base_down0,
					image_id::race_human_base_down1,
					image_id::race_human_base_down2,
					image_id::race_human_base_down3
				},
				.fps = 6,
				.loop = true
			}}
		}
	};
}
