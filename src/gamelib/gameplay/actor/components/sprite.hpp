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
inline std::size_t actor_component_mount<actor_component_id::sprite>
(
	const actor_component<actor_component_id::sprite>& component,
	std::span<quad_renderer::quad_data> quads
)
{
	std::size_t tex_count = component.data().textures.size();
	tz::assert(quads.size() >= tex_count, "Not enough quads provided (%zu) to actor component mount. Required %zu for this component", quads.size(), tex_count);
	for(std::size_t i = 0; i < tex_count; i++)
	{
		const sprite_texture_info& texture = component.data().textures[i];
		quads[i].pos = texture.offset;
		quads[i].texid = texture.id;
	}
	return tex_count;
}
