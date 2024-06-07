#include "gamelib/lua/meta.hpp"
#include "tz/lua/api.hpp"

namespace game::meta
{
	std::vector<modinfo_t> internal_mod_list = {};
	std::vector<prefabinfo_t> internal_prefab_list = {};
	std::vector<levelinfo_t> internal_level_list = {};
	std::vector<spellinfo_t> internal_spell_list = {};
	std::vector<buffinfo_t> internal_buff_list = {};
	std::vector<iteminfo_t> internal_item_list = {};

	LUA_BEGIN(rn_inform_mod)
		auto [name, description] = tz::lua::parse_args<std::string, std::string>(state);
		internal_mod_list.push_back({.name = name, .description = description});
		return 0;
	LUA_END

	LUA_BEGIN(rn_inform_prefab)
		auto [name, description, mod, has_static_init, has_pre_instantiate, has_instantiate, has_update, has_on_collision] = tz::lua::parse_args<std::string, std::string, std::string, bool, bool, bool, bool, bool>(state);
		auto iter = std::find_if(internal_mod_list.begin(), internal_mod_list.end(),
		[&mod](const auto& cur_mod)
		{
			return cur_mod.name == mod;
		});
		tz::assert(iter != internal_mod_list.end());
		std::size_t mod_id = std::distance(internal_mod_list.begin(), iter);
		internal_prefab_list.push_back({.name = name, .description = description, .mod_id = mod_id, .has_static_init = has_static_init, .has_pre_instantiate = has_pre_instantiate, .has_instantiate = has_instantiate, .has_update = has_update, .has_on_collision = has_on_collision});
		return 0;
	LUA_END

	LUA_BEGIN(rn_inform_level)
		auto [name, mod] = tz::lua::parse_args<std::string, std::string>(state);
		auto iter = std::find_if(internal_mod_list.begin(), internal_mod_list.end(),
		[&mod](const auto& cur_mod)
		{
			return cur_mod.name == mod;
		});
		tz::assert(iter != internal_mod_list.end());
		std::size_t mod_id = std::distance(internal_mod_list.begin(), iter);
		internal_level_list.push_back({.name = name, .mod_id = mod_id});
		return 0;
	LUA_END

	LUA_BEGIN(rn_inform_spell)
		auto [name, mod, description, magic_type, slot] = tz::lua::parse_args<std::string, std::string, std::string, std::string, std::string>(state);
		auto iter = std::find_if(internal_mod_list.begin(), internal_mod_list.end(),
		[&mod](const auto& cur_mod)
		{
			return cur_mod.name == mod;
		});
		tz::assert(iter != internal_mod_list.end());
		std::size_t mod_id = std::distance(internal_mod_list.begin(), iter);
		internal_spell_list.push_back({.name = name, .mod_id = mod_id, .description = description, .magic_type = magic_type, .slot = slot});
		return 0;
	LUA_END

	LUA_BEGIN(rn_inform_buff)
		auto [name, mod, description, duration, is_internal] = tz::lua::parse_args<std::string, std::string, std::string, int, bool>(state);
		auto iter = std::find_if(internal_mod_list.begin(), internal_mod_list.end(),
		[&mod](const auto& cur_mod)
		{
			return cur_mod.name == mod;
		});
		tz::assert(iter != internal_mod_list.end());
		std::size_t mod_id = std::distance(internal_mod_list.begin(), iter);
		internal_buff_list.push_back({.name = name, .mod_id = mod_id, .description = description, .duration = duration, .is_internal = is_internal});
		return 0;
	LUA_END

	LUA_BEGIN(rn_inform_item)
		auto [name, mod, tooltip, slot_id, rarity, rarity_r, rarity_g, rarity_b] = tz::lua::parse_args<std::string, std::string, std::string, unsigned int, std::string, float, float, float>(state);
		auto iter = std::find_if(internal_mod_list.begin(), internal_mod_list.end(),
		[&mod](const auto& cur_mod)
		{
			return cur_mod.name == mod;
		});
		tz::assert(iter != internal_mod_list.end());
		std::size_t mod_id = std::distance(internal_mod_list.begin(), iter);
		internal_item_list.push_back({.name = name, .mod_id = mod_id, .tooltip = tooltip, .slot_id = slot_id, .rarity = rarity, .rarity_colour = tz::vec3{rarity_r, rarity_g, rarity_b}});
		return 0;
	LUA_END

	std::span<modinfo_t> get_mods()
	{
		return internal_mod_list;
	}

	std::span<prefabinfo_t> get_prefabs()
	{
		return internal_prefab_list;
	}

	std::span<levelinfo_t> get_levels()
	{
		return internal_level_list;
	}

	std::span<spellinfo_t> get_spells()
	{
		return internal_spell_list;
	}

	std::span<buffinfo_t> get_buffs()
	{
		return internal_buff_list;
	}

	std::span<iteminfo_t> get_items()
	{
		return internal_item_list;
	}

	void lua_initialise(tz::lua::state& state)
	{
		state.assign_func("rn.inform_mod", LUA_FN_NAME(rn_inform_mod));
		state.assign_func("rn.inform_prefab", LUA_FN_NAME(rn_inform_prefab));
		state.assign_func("rn.inform_level", LUA_FN_NAME(rn_inform_level));
		state.assign_func("rn.inform_spell", LUA_FN_NAME(rn_inform_spell));
		state.assign_func("rn.inform_buff", LUA_FN_NAME(rn_inform_buff));
		state.assign_func("rn.inform_item", LUA_FN_NAME(rn_inform_item));
	}

	void reflect()
	{
		internal_mod_list.clear();
		internal_prefab_list.clear();
		internal_level_list.clear();
		internal_spell_list.clear();
		internal_buff_list.clear();
		internal_item_list.clear();
		tz::lua::get_state().execute(R"(
			-- reflect mods
			for modname, moddata in pairs(rn.mods) do
				rn.inform_mod(modname, moddata.description)
			end

			-- reflect prefabs
			for prefabname, prefabdata in pairs(rn.entity.prefabs) do
				rn.inform_prefab(prefabname, prefabdata.description or "<No Description>", prefabdata.mod, prefabdata.static_init ~= nil, prefabdata.pre_instantiate ~= nil, prefabdata.instantiate ~= nil, prefabdata.update ~= nil, prefabdata.on_collision ~= nil)
			end

			-- reflect levels
			for levelname, leveldata in pairs(rn.level.levels) do
				rn.inform_level(levelname, leveldata.mod)
			end

			-- reflect spells
			for spellname, spelldata in pairs(rn.spell.spells) do
				rn.inform_spell(spellname, spelldata.mod, spelldata.description or "<No Description>", spelldata.magic_type or "Untyped", spelldata.slot or "<No Slot>")
			end

			-- reflect buffs
			for buffname, buffdata in pairs(rn.buff.buffs) do
				local is_internal = true
				if buffdata.internal ~= nil then is_internal = buffdata.internal end
				rn.inform_buff(buffname, buffdata.mod, buffdata.description or "<No Description>", buffdata.duration or -1, is_internal)
			end

			-- reflect items
			for itemname, itemdata in pairs(rn.item.items) do
				local rarity = itemdata.rarity or "common"
				local rarity_name = rn.item.rarity[rarity].pretty_name
				local rarity_colour = rn.item.rarity[rarity].colour
				rn.inform_item(itemname, itemdata.mod, itemdata.tooltip or "<No Tooltip>", itemdata.slot or rn.item.slot.none, rarity_name, rarity_colour[1], rarity_colour[2], rarity_colour[3])
			end
		)");
	}
}