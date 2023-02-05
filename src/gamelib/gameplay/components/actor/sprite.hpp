template<>
struct actor_component_params<actor_component_id::sprite>
{
	std::uint32_t texture_id;
};

template<>
inline void actor_component_mount<actor_component_id::sprite>
(
	const actor_component<actor_component_id::sprite>& component,
	quad_renderer::quad_data& quad
)
{
	quad.texid = component.data().texture_id;
}
