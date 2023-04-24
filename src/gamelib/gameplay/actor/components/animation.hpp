template<>
struct actor_component_params<actor_component_id::animation>
{
	std::vector<std::optional<animation>> animations = {};
	float animation_speed = 1.0f;
};

template<>
inline void actor_component_update<actor_component_id::animation>
(
	actor_component<actor_component_id::animation>& component,
	float dt,
	actor& actor
)
{
	tz::assert(actor.entity.has_component<actor_component_id::sprite>(), "`animation` component detected on an entity, but the entity doesn't have a `sprite` component to manipulate!");
	auto& sprite_comp = *actor.entity.get_component<actor_component_id::sprite>();
	auto& sprite_textures = sprite_comp.data().textures;
	tz::assert(component.data().animations.size() == sprite_textures.size(), "Detected `animation` component on an entity with %zu animations attached. This needs to exactly match the number of textures attached to its `sprite` component (which is currently %zu)", component.data().animations.size(), sprite_textures.size());
	component.data().animations.resize(sprite_textures.size());
	for(std::size_t i = 0; i < component.data().animations.size(); i++)
	{
		auto& animation = component.data().animations[i];
		
		if(!animation.has_value())
		{
			continue;
		}
		animation->update(dt * component.data().animation_speed);
		auto& sprite = sprite_textures[i];
		sprite.id = animation->get_image();
	}
}

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::animation>& component)
{
	if(component.data().animations.empty())
	{
		return;
	}
	static int anim_id = 0;
	ImGui::SliderInt("Animation ID", &anim_id, 0, component.data().animations.size() - 1);
	ImGui::Indent();
	if(!component.data().animations[anim_id].has_value())
	{
		ImGui::Text("<no data>");
	}
	else
	{
		component.data().animations[anim_id]->dbgui();
	}
	ImGui::Unindent();
}
