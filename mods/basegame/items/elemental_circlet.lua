rn.mods.basegame.items.elemental_circlet_base = 
{
	static_init = function()
		rn.renderer():add_texture("helm.circlet", "basegame/res/skins/helms/circlet.png")
	end,
	tooltip = [[
		+20% Physical Resist
		+2 Health per 5 seconds
	]],
	slot = rn.item.slot.helm,
	colour = {1.0, 1.0, 1.0},
	rarity = "common",
	texture = "helm.circlet"
}

for schoolname, schooldata in pairs(rn.spell.schools) do
	if schoolname ~= "physical" then
		-- is a magic school.
		rn.mods.basegame.items["elemental_circlet_" .. schoolname] =
		{
			on_equip = function(uuid)
				rn.entity.prefabs.combat_stats["apply_flat_increased_" .. schoolname .. "_resist"](uuid, 0.5)
				rn.entity.prefabs.combat_stats["apply_flat_increased_" .. schoolname .. "_power"](uuid, 1.0)
				rn.entity.prefabs.combat_stats["apply_pct_increased_" .. schoolname .. "_power"](uuid, 0.25)
			end,
			on_unequip = function(uuid)
				rn.entity.prefabs.combat_stats["apply_flat_increased_" .. schoolname .. "_resist"](uuid, -0.5)
				rn.entity.prefabs.combat_stats["apply_flat_increased_" .. schoolname .. "_power"](uuid, -1.0)
				rn.entity.prefabs.combat_stats["apply_pct_increased_" .. schoolname .. "_power"](uuid, -0.25)
			end,
			slot = rn.mods.basegame.items.elemental_circlet_base.slot,
			colour = rn.spell.schools[schoolname].colour,
			rarity = "epic",
			texture = rn.mods.basegame.items.elemental_circlet_base.texture
		}
	end
end