template<>
struct actor_component_params<actor_component_id::animation>
{
	std::vector<std::optional<animation>> animations = {};
};

template<>
inline void actor_component_update<actor_component_id::animation>
(
	actor_component<actor_component_id::animation>& component,
	float dt,
	actor_entity& entity
)
{
	tz::assert(entity.has_component<actor_component_id::sprite>(), "`animation` component detected on an entity, but the entity doesn't have a `sprite` component to manipulate!");
	auto& sprite_comp = *entity.get_component<actor_component_id::sprite>();
	auto& sprite_textures = sprite_comp.data().textures;
	tz::assert(component.data().animations.size() == sprite_textures.size(), "Detected `animation` component on an entity with %zu animations attached. This needs to exactly match the number of textures attached to its `sprite` component (which is currently %zu)", component.data().animations.size(), sprite_textures.size());
	for(std::size_t i = 0; i < component.data().animations.size(); i++)
	{
		auto& animation = component.data().animations[i];
		
		if(!animation.has_value())
		{
			continue;
		}
		animation->update(dt);
		auto& sprite = sprite_textures[i];
		sprite.id = animation->get_image();
	}
}
