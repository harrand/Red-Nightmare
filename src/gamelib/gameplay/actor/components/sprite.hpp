struct sprite_texture_info
{
	std::uint32_t id = 0;
	tz::vec2 offset = tz::vec2::zero();
};

template<>
struct actor_component_params<actor_component_id::sprite>
{
	std::vector<sprite_texture_info> textures = {};
};

template<>
inline mount_result actor_component_mount<actor_component_id::sprite>
(
	const actor_component<actor_component_id::sprite>& component,
	std::span<quad_renderer::quad_data> quads
)
{
	std::size_t tex_count = component.data().textures.size();
	if(quads.size() <= tex_count)
	{
		return {.error = mount_error::ooq};
	}
	for(std::size_t i = 0; i < tex_count; i++)
	{
		const sprite_texture_info& texture = component.data().textures[i];
		quads[i].pos = texture.offset;
		quads[i].texid = texture.id;
	}
	return {.count = tex_count};
}
