#ifndef RN_GAMELIB_RENDERER_TEXTURE_MANAGER_HPP
#define RN_GAMELIB_RENDERER_TEXTURE_MANAGER_HPP
#include "tz/lua/api.hpp"
#include "tz/ren/animation2.hpp"
#include <string>
#include <unordered_map>

namespace game::render
{
	class texture_manager
	{
	public:
		texture_manager(tz::ren::animation_renderer2& anim_ren);
		void lua_initialise(tz::lua::state& state);

		void register_texture(std::string name, std::string_view texture_path);
		tz::ren::animation_renderer2::texture_handle get_texture(const std::string& name) const;
		bool has_texture(const std::string& name) const;
	private:
		tz::ren::animation_renderer2* anim_ren = nullptr;
		std::unordered_map<std::string, tz::ren::animation_renderer2::texture_handle> stored_textures = {};
	};

	struct impl_rn_texture_manager
	{
		texture_manager* texmgr;
		int register_texture(tz::lua::state& state);
		int get_texture(tz::lua::state& state);
		int has_texture(tz::lua::state& state);
	};

	LUA_CLASS_BEGIN(impl_rn_texture_manager)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(impl_rn_texture_manager, register_texture)
			LUA_METHOD(impl_rn_texture_manager, get_texture)
			LUA_METHOD(impl_rn_texture_manager, has_texture)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END
}

#endif // RN_GAMELIB_RENDERER_TEXTURE_MANAGER_HPP