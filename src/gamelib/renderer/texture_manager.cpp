#include "gamelib/renderer/texture_manager.hpp"
#include "tz/core/profile.hpp"

namespace game::render
{
	texture_manager::texture_manager(tz::ren::animation_renderer2& anim_ren):
	anim_ren(&anim_ren)
	{
	}

	void texture_manager::register_texture(std::string name, std::string_view texture_path)
	{
		TZ_PROFZONE("texture manager - register texture", 0xFF11CC88);
		auto iter = this->stored_textures.find(name);
		if(TZ_DEBUG && iter != this->stored_textures.end())
		{
			tz::report("Warning: Attempt to register a texture of name \"%s\", but it already existed.", name.c_str());
			return;
		}

		tz::assert(this->anim_ren != nullptr);
		auto texh = this->anim_ren->add_texture(tz::io::image::load_from_file(texture_path));
		this->stored_textures[name] = texh;
	}

	tz::ren::animation_renderer2::texture_handle texture_manager::get_texture(const std::string& name) const
	{
		TZ_PROFZONE("texture manager - get texture", 0xFF11CC88);
		auto iter = this->stored_textures.find(name);
		if(iter != this->stored_textures.end())
		{
			return iter->second;
		}
		return tz::nullhand;
	}

	bool texture_manager::has_texture(const std::string& name) const
	{
		return this->get_texture(name) != tz::nullhand;
	}

	// LUA API
	int impl_rn_texture_manager::register_texture(tz::lua::state& state)
	{
		auto [_, name, path] = tz::lua::parse_args<tz::lua::nil, std::string, std::string>(state);
		this->texmgr->register_texture(name, path);
		return 0;
	}

	int impl_rn_texture_manager::get_texture(tz::lua::state& state)
	{
		auto [_, name] = tz::lua::parse_args<tz::lua::nil, std::string>(state);
		tz::ren::animation_renderer2::texture_handle texh = this->texmgr->get_texture(name);
		state.stack_push_uint(static_cast<std::size_t>(static_cast<tz::hanval>(texh)));
		return 1;
	}

	int impl_rn_texture_manager::has_texture(tz::lua::state& state)
	{
		auto [_, name] = tz::lua::parse_args<tz::lua::nil, std::string>(state);
		state.stack_push_bool(this->texmgr->has_texture(name));
		return 1;
	}

	void texture_manager::lua_initialise(tz::lua::state& state)
	{
		state.new_type("impl_rn_texture_manager", LUA_CLASS_NAME(impl_rn_texture_manager)::registers);
	}
}