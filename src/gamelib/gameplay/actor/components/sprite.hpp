#include "gamelib/render/shaders/quadtex.tzsl"

struct sprite_texture_info
{
	std::uint32_t id = 0;
	tz::vec2 offset = tz::vec2::zero();
	tz::vec3 colour_tint = tz::vec3::filled(1.0f);
	unsigned int layer_offset = 0;
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
	std::vector<sprite_texture_info> sorted_textures = component.data().textures;
	std::sort(sorted_textures.begin(), sorted_textures.end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs.layer_offset < rhs.layer_offset;
	});
	auto& quad = quads[0];
	// each texture has its own layer.
	// 0th texture needs to be highest layer, and vice versa.
	for(std::size_t i = 0; i < tex_count; i++)
	{
		quad.texid[i] = sorted_textures[i].id;
		quad.tints[i] = sorted_textures[i].colour_tint.with_more(0.0f);
	}
	for(std::size_t i = tex_count; i < QUAD_TEXCOUNT; i++)
	{
		quad.texid[i] = 1u;
	}
	return {.count = 1};
}

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::sprite>& component)
{
	static int texture_index = 0;
	if(component.data().textures.empty())
	{
		ImGui::Text("No textures attached to this sprite!");
		return;
	}
	ImGui::SliderInt("Texture Index", &texture_index, 0, component.data().textures.size() - 1);
	ImGui::Indent();
	auto& tex = component.data().textures[texture_index];
	ImGui::Text("Texture ID: %zu", tex.id);
	ImGui::InputFloat2("Offset", tex.offset.data().data());
	ImGui::SliderFloat3("Colour Tint", tex.colour_tint.data().data(), 0.0f, 1.0f);
	// todo: no magic number.
	ImGui::SliderInt("Layer Offset", reinterpret_cast<int*>(&tex.layer_offset), 0, 1000, "%u");
	ImGui::Unindent();
}
