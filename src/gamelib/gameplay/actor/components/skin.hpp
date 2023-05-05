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
	bool impl_dirty = false;
	struct skin_cache_t
	{
		prefab::human_art::hair_style hair_style;
		prefab::human_art::eye_type eye_type;
		prefab::human_art::helm_type helm_type;
		prefab::human_art::chest_type chest_type;
		prefab::human_art::feet_type feet_type;
	} skin_cache;
};

template<>
inline void actor_component_update<actor_component_id::skin>
(
	actor_component<actor_component_id::skin>& component,
	float dt,
	actor& actor
)
{
	TZ_PROFZONE("skin component - update", 0xffaa0077);
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

	{
		TZ_PROFZONE("sprite colour writes", 0xffaa0077);
		auto& sprite = *actor.entity.get_component<actor_component_id::sprite>();
		sprite.data().textures[0].colour_tint = component.data().data.equipment.helm_colour;
		sprite.data().textures[1].colour_tint = component.data().data.hair_colour;
		sprite.data().textures[2].colour_tint = component.data().data.eye_colour;
		sprite.data().textures[3].colour_tint = component.data().data.equipment.chest_colour;
		sprite.data().textures[4].colour_tint = component.data().data.equipment.feet_colour;
		sprite.data().textures[5].colour_tint = component.data().data.skin_colour;
	}
	auto& skel = *actor.entity.get_component<actor_component_id::humanoid_skeleton>();
	if(component.data().impl_dirty)
	{
		skel.data().should_repaint = true;
		component.data().impl_dirty = false;
	}
	auto get_hair_final = [](prefab::human_art::hair_style style, humanoid_skeleton_animation anim, prefab::human_art::helm_type helm)
	{
		if(prefab::human_art::helm_hides_hair(helm))
		{
			return prefab::human_art::get_hair(prefab::human_art::hair_style::bald, anim);
		}
		return prefab::human_art::get_hair(style, anim);
	};
	bool cache_miss = false;
	cache_miss |= (component.data().skin_cache.hair_style != component.data().data.hair_style);
	cache_miss |= (component.data().skin_cache.eye_type != component.data().data.eye_type);
	cache_miss |= (component.data().skin_cache.helm_type != component.data().data.equipment.helm_type);
	cache_miss |= (component.data().skin_cache.chest_type != component.data().data.equipment.chest_type);
	cache_miss |= (component.data().skin_cache.feet_type != component.data().data.equipment.feet_type);
	if(cache_miss)
	{
		component.data().skin_cache.hair_style = component.data().data.hair_style;
		component.data().skin_cache.eye_type = component.data().data.eye_type;
		component.data().skin_cache.helm_type = component.data().data.equipment.helm_type;
		component.data().skin_cache.chest_type = component.data().data.equipment.chest_type;
		component.data().skin_cache.feet_type = component.data().data.equipment.feet_type;
		TZ_PROFZONE("pose updates", 0xffaa0077);
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
		// cast
		skel.data().poses[4] = actor_component_params<actor_component_id::animation>
		{
			.animations =
			{
				prefab::human_art::get_helm(component.data().data.equipment.helm_type, humanoid_skeleton_animation::cast),
				get_hair_final(component.data().data.hair_style, humanoid_skeleton_animation::cast, component.data().data.equipment.helm_type),
				prefab::human_art::get_eyes(component.data().data.eye_type, humanoid_skeleton_animation::cast),
				prefab::human_art::get_chest(component.data().data.equipment.chest_type, humanoid_skeleton_animation::cast),
				prefab::human_art::get_feet(component.data().data.equipment.feet_type, humanoid_skeleton_animation::cast),
				animation
				{{
					.frame_textures =
					{
						image_id::race_human_base_cast0,
						image_id::race_human_base_cast1,
						image_id::race_human_base_cast2,
						image_id::race_human_base_cast3,
						image_id::race_human_base_cast4,
						image_id::race_human_base_cast3,
						image_id::race_human_base_cast2,
						image_id::race_human_base_cast1,
					},
					.fps = 7,
					.loop = true
				}}
			}
		};
	}
}

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::skin>& component)
{
	ImGui::SliderFloat3("Skin Colour", component.data().data.skin_colour.data().data(), 0.0f, 1.0f);
	ImGui::Spacing();
	component.data().impl_dirty |= ImGui::SliderInt("Hair Style", reinterpret_cast<int*>(&component.data().data.hair_style), 0, static_cast<int>(prefab::human_art::hair_style::_count) - 1);
	ImGui::SliderFloat3("Hair Colour", component.data().data.hair_colour.data().data(), 0.0f, 1.0f);
	ImGui::Spacing();
	ImGui::SliderFloat3("Eye Colour", component.data().data.eye_colour.data().data(), 0.0f, 1.0f);
	ImGui::Spacing();
	component.data().impl_dirty |= ImGui::SliderInt("Helm", reinterpret_cast<int*>(&component.data().data.equipment.helm_type), 0, static_cast<int>(prefab::human_art::helm_type::_count) - 1);
	ImGui::SliderFloat3("Helm Colour", component.data().data.equipment.helm_colour.data().data(), 0.0f, 1.0f);
	ImGui::Spacing();
	component.data().impl_dirty |= ImGui::SliderInt("Chest", reinterpret_cast<int*>(&component.data().data.equipment.chest_type), 0, static_cast<int>(prefab::human_art::chest_type::_count) - 1);
	ImGui::SliderFloat3("Chest Colour", component.data().data.equipment.chest_colour.data().data(), 0.0f, 1.0f);
	ImGui::Spacing();
}
