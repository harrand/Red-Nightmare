rn = rn or {}
rn.mods = rn.mods or {}
rn.entity = rn.entity or {}
rn.entity.prefabs = rn.entity.prefabs or {}

rn.load_mods = function()
	for modname, moddata in pairs(rn.mods) do

		-- install all prefabs provided by the mod
		for prefabname, prefabdata in pairs(moddata.prefabs) do
			-- note: prefab.mod will show the name of the mod that provided the prefab
			-- however, it's quite possible for prefabs to not come from any particular mods.
			-- those would be called default prefabs.
			prefabdata.mod = modname
			rn.entity.prefabs[prefabname] = prefabdata
		end

		for levelname, leveldata in pairs(moddata.levels) do
			leveldata.mod = modname
			rn.level.levels[levelname] = leveldata
		end

		for spellname, spelldata in pairs(moddata.spells) do
			spelldata.mod = modname
			rn.spell.spells[spellname] = spelldata
		end

		for itemname, itemdata in pairs(moddata.items) do
			itemdata.mod = modname
			rn.item.items[itemname] = itemdata
			local rarity = itemdata.rarity or "common"
			if itemdata.valid_loot == true or itemdata.valid_loot == nil then
				table.insert(rn.item.rarity[rarity].items, itemname)
			end
		end
	end
end