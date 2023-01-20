#include "images.hpp"
#include "tz/core/imported_text.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Note: When adding a new image, add a new entry here and in game::texture_data below.
#include ImportedTextHeader(missing, png)
#include ImportedTextHeader(invisible, png)
#include ImportedTextHeader(smooth_normals, png)
#include ImportedTextHeader(player_classic_dead, png)
#include ImportedTextHeader(player_classic_down1, png)
#include ImportedTextHeader(player_classic_down2, png)
#include ImportedTextHeader(player_classic_down3, png)
#include ImportedTextHeader(player_classic_dying1, png)
#include ImportedTextHeader(player_classic_dying2, png)
#include ImportedTextHeader(player_classic_dying3, png)
#include ImportedTextHeader(player_classic_dying4, png)
#include ImportedTextHeader(player_classic_dying5, png)
#include ImportedTextHeader(player_classic_dying6, png)
#include ImportedTextHeader(player_classic_dying7, png)
#include ImportedTextHeader(player_classic_dying8, png)
#include ImportedTextHeader(player_classic_idle1, png)
#include ImportedTextHeader(player_classic_idle2, png)
#include ImportedTextHeader(player_classic_side1, png)
#include ImportedTextHeader(player_classic_side2, png)
#include ImportedTextHeader(player_classic_side3, png)
#include ImportedTextHeader(player_classic_special, png)
#include ImportedTextHeader(player_classic_up1, png)
#include ImportedTextHeader(player_classic_up2, png)
#include ImportedTextHeader(player_classic_up3, png)

#include ImportedTextHeader(player_classic_lowpoly_cast0, png)
#include ImportedTextHeader(player_classic_lowpoly_cast1, png)
#include ImportedTextHeader(player_classic_lowpoly_cast2, png)
#include ImportedTextHeader(player_classic_lowpoly_cast3, png)
#include ImportedTextHeader(player_classic_lowpoly_cast4, png)
#include ImportedTextHeader(player_classic_lowpoly_cast5, png)
#include ImportedTextHeader(player_classic_lowpoly_cast6, png)
#include ImportedTextHeader(player_classic_lowpoly_cast7, png)
#include ImportedTextHeader(player_classic_lowpoly_death00, png)
#include ImportedTextHeader(player_classic_lowpoly_death01, png)
#include ImportedTextHeader(player_classic_lowpoly_death02, png)
#include ImportedTextHeader(player_classic_lowpoly_death03, png)
#include ImportedTextHeader(player_classic_lowpoly_death04, png)
#include ImportedTextHeader(player_classic_lowpoly_death05, png)
#include ImportedTextHeader(player_classic_lowpoly_death06, png)
#include ImportedTextHeader(player_classic_lowpoly_death07, png)
#include ImportedTextHeader(player_classic_lowpoly_death08, png)
#include ImportedTextHeader(player_classic_lowpoly_death09, png)
#include ImportedTextHeader(player_classic_lowpoly_death10, png)
#include ImportedTextHeader(player_classic_lowpoly_death11, png)
#include ImportedTextHeader(player_classic_lowpoly_down0, png)
#include ImportedTextHeader(player_classic_lowpoly_down1, png)
#include ImportedTextHeader(player_classic_lowpoly_down2, png)
#include ImportedTextHeader(player_classic_lowpoly_down3, png)
#include ImportedTextHeader(player_classic_lowpoly_endcast0, png)
#include ImportedTextHeader(player_classic_lowpoly_endcast1, png)
#include ImportedTextHeader(player_classic_lowpoly_endcast2, png)
#include ImportedTextHeader(player_classic_lowpoly_endcast3, png)
#include ImportedTextHeader(player_classic_lowpoly_endcast4, png)
#include ImportedTextHeader(player_classic_lowpoly_endcast5, png)
#include ImportedTextHeader(player_classic_lowpoly_idle0, png)
#include ImportedTextHeader(player_classic_lowpoly_idle1, png)
#include ImportedTextHeader(player_classic_lowpoly_side0, png)
#include ImportedTextHeader(player_classic_lowpoly_side1, png)
#include ImportedTextHeader(player_classic_lowpoly_side2, png)
#include ImportedTextHeader(player_classic_lowpoly_side3, png)
#include ImportedTextHeader(player_classic_lowpoly_up0, png)
#include ImportedTextHeader(player_classic_lowpoly_up1, png)
#include ImportedTextHeader(player_classic_lowpoly_up2, png)
#include ImportedTextHeader(player_classic_lowpoly_up3, png)

#include ImportedTextHeader(ghost_zombie_death0, png)
#include ImportedTextHeader(ghost_zombie_death1, png)
#include ImportedTextHeader(ghost_zombie_death2, png)
#include ImportedTextHeader(ghost_zombie_death3, png)
#include ImportedTextHeader(ghost_zombie_down0, png)
#include ImportedTextHeader(ghost_zombie_down1, png)
#include ImportedTextHeader(ghost_zombie_down2, png)
#include ImportedTextHeader(ghost_zombie_down3, png)
#include ImportedTextHeader(ghost_zombie_idle0, png)
#include ImportedTextHeader(ghost_zombie_idle1, png)
#include ImportedTextHeader(ghost_zombie_side0, png)
#include ImportedTextHeader(ghost_zombie_side1, png)
#include ImportedTextHeader(ghost_zombie_side2, png)
#include ImportedTextHeader(ghost_zombie_side3, png)
#include ImportedTextHeader(ghost_zombie_up0, png)
#include ImportedTextHeader(ghost_zombie_up1, png)
#include ImportedTextHeader(ghost_zombie_up2, png)
#include ImportedTextHeader(ghost_zombie_up3, png)

#include ImportedTextHeader(mjzombie_death0, png)
#include ImportedTextHeader(mjzombie_death1, png)
#include ImportedTextHeader(mjzombie_death2, png)
#include ImportedTextHeader(mjzombie_death3, png)
#include ImportedTextHeader(mjzombie_down0, png)
#include ImportedTextHeader(mjzombie_down1, png)
#include ImportedTextHeader(mjzombie_down2, png)
#include ImportedTextHeader(mjzombie_down3, png)
#include ImportedTextHeader(mjzombie_idle0, png)
#include ImportedTextHeader(mjzombie_idle1, png)
#include ImportedTextHeader(mjzombie_side0, png)
#include ImportedTextHeader(mjzombie_side1, png)
#include ImportedTextHeader(mjzombie_side2, png)
#include ImportedTextHeader(mjzombie_side3, png)
#include ImportedTextHeader(mjzombie_up0, png)
#include ImportedTextHeader(mjzombie_up1, png)
#include ImportedTextHeader(mjzombie_up2, png)
#include ImportedTextHeader(mjzombie_up3, png)

#include ImportedTextHeader(banshee_idle0, png)
#include ImportedTextHeader(banshee_idle1, png)
#include ImportedTextHeader(banshee_death00, png)
#include ImportedTextHeader(banshee_death01, png)
#include ImportedTextHeader(banshee_death02, png)
#include ImportedTextHeader(banshee_death03, png)
#include ImportedTextHeader(banshee_death04, png)
#include ImportedTextHeader(banshee_death05, png)
#include ImportedTextHeader(banshee_death06, png)
#include ImportedTextHeader(banshee_death07, png)
#include ImportedTextHeader(banshee_death08, png)
#include ImportedTextHeader(banshee_death09, png)
#include ImportedTextHeader(banshee_death10, png)
#include ImportedTextHeader(banshee_death11, png)

#include ImportedTextHeader(player_classic_default_fireball, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_0, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_1, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_2, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_3, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_4, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_5, png)
#include ImportedTextHeader(player_classic_default_fireball_particles_6, png)

#include ImportedTextHeader(block_break00, png)
#include ImportedTextHeader(block_break01, png)
#include ImportedTextHeader(block_break02, png)
#include ImportedTextHeader(block_break03, png)
#include ImportedTextHeader(block_break04, png)
#include ImportedTextHeader(block_break05, png)
#include ImportedTextHeader(block_break06, png)
#include ImportedTextHeader(block_break07, png)
#include ImportedTextHeader(block_break08, png)
#include ImportedTextHeader(block_break09, png)
#include ImportedTextHeader(block_break10, png)
#include ImportedTextHeader(block_break11, png)
#include ImportedTextHeader(block_break12, png)
#include ImportedTextHeader(block_break13, png)

#include ImportedTextHeader(blood_splatter0, png)
#include ImportedTextHeader(blood_splatter1, png)
#include ImportedTextHeader(blood_splatter2, png)
#include ImportedTextHeader(blood_splatter3, png)
#include ImportedTextHeader(blood_splatter4, png)
#include ImportedTextHeader(blood_splatter5, png)
#include ImportedTextHeader(blood_splatter6, png)
#include ImportedTextHeader(blood_splatter7, png)

#include ImportedTextHeader(fire_explosion_0, png)
#include ImportedTextHeader(fire_explosion_1, png)
#include ImportedTextHeader(fire_explosion_2, png)
#include ImportedTextHeader(fire_explosion_3, png)
#include ImportedTextHeader(fire_explosion_4, png)
#include ImportedTextHeader(fire_explosion_5, png)
#include ImportedTextHeader(fire_explosion_6, png)

#include ImportedTextHeader(nightmare_dead, png)
#include ImportedTextHeader(nightmare_down1, png)
#include ImportedTextHeader(nightmare_down2, png)
#include ImportedTextHeader(nightmare_down3, png)
#include ImportedTextHeader(nightmare_dying1, png)
#include ImportedTextHeader(nightmare_dying2, png)
#include ImportedTextHeader(nightmare_dying3, png)
#include ImportedTextHeader(nightmare_dying4, png)
#include ImportedTextHeader(nightmare_dying5, png)
#include ImportedTextHeader(nightmare_dying6, png)
#include ImportedTextHeader(nightmare_dying7, png)
#include ImportedTextHeader(nightmare_dying8, png)
#include ImportedTextHeader(nightmare_dying9, png)
#include ImportedTextHeader(nightmare_dying10, png)
#include ImportedTextHeader(nightmare_idle1, png)
#include ImportedTextHeader(nightmare_idle2, png)
#include ImportedTextHeader(nightmare_side1, png)
#include ImportedTextHeader(nightmare_side2, png)
#include ImportedTextHeader(nightmare_side3, png)
#include ImportedTextHeader(nightmare_special, png)
#include ImportedTextHeader(nightmare_up1, png)
#include ImportedTextHeader(nightmare_up2, png)
#include ImportedTextHeader(nightmare_up3, png)

#include ImportedTextHeader(material_stone, png)
#include ImportedTextHeader(material_stone_normal, png)
#include ImportedTextHeader(healthbar_empty, png)
#include ImportedTextHeader(gravestone_0, png)
#include ImportedTextHeader(gravestone_1, png)
#include ImportedTextHeader(torch_off, png)
#include ImportedTextHeader(downwards_trapdoor, png)
#include ImportedTextHeader(stone_stairs_down_nx, png)
#include ImportedTextHeader(stone_stairs_up_nx, png)
#include ImportedTextHeader(stone_stairs_down_py, png)
#include ImportedTextHeader(stone_stairs_up_py, png)

#include ImportedTextHeader(sprint0, png)
#include ImportedTextHeader(sprint1, png)
#include ImportedTextHeader(sprint2, png)

#include ImportedTextHeader(dev_level_1_backdrop, png)
#include ImportedTextHeader(dev_level_1_backdrop_normal, png)
#include ImportedTextHeader(grass_backdrop, png)
#include ImportedTextHeader(grass_backdrop_normal, png)
#include ImportedTextHeader(snow_backdrop, png)
#include ImportedTextHeader(dungeon_floor_backdrop, png)
#include ImportedTextHeader(dungeon_floor_backdrop_normal, png)

namespace game
{
	std::array<std::string_view, static_cast<int>(TextureID::Count)> texture_data
	{
		ImportedTextData(missing, png),
		ImportedTextData(invisible, png),
		ImportedTextData(smooth_normals, png),
		ImportedTextData(player_classic_dead, png),
		ImportedTextData(player_classic_down1, png),
		ImportedTextData(player_classic_down2, png),
		ImportedTextData(player_classic_down3, png),
		ImportedTextData(player_classic_dying1, png),
		ImportedTextData(player_classic_dying2, png),
		ImportedTextData(player_classic_dying3, png),
		ImportedTextData(player_classic_dying4, png),
		ImportedTextData(player_classic_dying5, png),
		ImportedTextData(player_classic_dying6, png),
		ImportedTextData(player_classic_dying7, png),
		ImportedTextData(player_classic_dying8, png),
		ImportedTextData(player_classic_idle1, png),
		ImportedTextData(player_classic_idle2, png),
		ImportedTextData(player_classic_side1, png),
		ImportedTextData(player_classic_side2, png),
		ImportedTextData(player_classic_side3, png),
		ImportedTextData(player_classic_special, png),
		ImportedTextData(player_classic_up1, png),
		ImportedTextData(player_classic_up2, png),
		ImportedTextData(player_classic_up3, png),

		ImportedTextData(player_classic_lowpoly_cast0, png),
		ImportedTextData(player_classic_lowpoly_cast1, png),
		ImportedTextData(player_classic_lowpoly_cast2, png),
		ImportedTextData(player_classic_lowpoly_cast3, png),
		ImportedTextData(player_classic_lowpoly_cast4, png),
		ImportedTextData(player_classic_lowpoly_cast5, png),
		ImportedTextData(player_classic_lowpoly_cast6, png),
		ImportedTextData(player_classic_lowpoly_cast7, png),
		ImportedTextData(player_classic_lowpoly_death00, png),
		ImportedTextData(player_classic_lowpoly_death01, png),
		ImportedTextData(player_classic_lowpoly_death02, png),
		ImportedTextData(player_classic_lowpoly_death03, png),
		ImportedTextData(player_classic_lowpoly_death04, png),
		ImportedTextData(player_classic_lowpoly_death05, png),
		ImportedTextData(player_classic_lowpoly_death06, png),
		ImportedTextData(player_classic_lowpoly_death07, png),
		ImportedTextData(player_classic_lowpoly_death08, png),
		ImportedTextData(player_classic_lowpoly_death09, png),
		ImportedTextData(player_classic_lowpoly_death10, png),
		ImportedTextData(player_classic_lowpoly_death11, png),
		ImportedTextData(player_classic_lowpoly_down0, png),
		ImportedTextData(player_classic_lowpoly_down1, png),
		ImportedTextData(player_classic_lowpoly_down2, png),
		ImportedTextData(player_classic_lowpoly_down3, png),
		ImportedTextData(player_classic_lowpoly_endcast0, png),
		ImportedTextData(player_classic_lowpoly_endcast1, png),
		ImportedTextData(player_classic_lowpoly_endcast2, png),
		ImportedTextData(player_classic_lowpoly_endcast3, png),
		ImportedTextData(player_classic_lowpoly_endcast4, png),
		ImportedTextData(player_classic_lowpoly_endcast5, png),
		ImportedTextData(player_classic_lowpoly_idle0, png),
		ImportedTextData(player_classic_lowpoly_idle1, png),
		ImportedTextData(player_classic_lowpoly_side0, png),
		ImportedTextData(player_classic_lowpoly_side1, png),
		ImportedTextData(player_classic_lowpoly_side2, png),
		ImportedTextData(player_classic_lowpoly_side3, png),
		ImportedTextData(player_classic_lowpoly_up0, png),
		ImportedTextData(player_classic_lowpoly_up1, png),
		ImportedTextData(player_classic_lowpoly_up2, png),
		ImportedTextData(player_classic_lowpoly_up3, png),

		ImportedTextData(ghost_zombie_death0, png),
		ImportedTextData(ghost_zombie_death1, png),
		ImportedTextData(ghost_zombie_death2, png),
		ImportedTextData(ghost_zombie_death3, png),
		ImportedTextData(ghost_zombie_down0, png),
		ImportedTextData(ghost_zombie_down1, png),
		ImportedTextData(ghost_zombie_down2, png),
		ImportedTextData(ghost_zombie_down3, png),
		ImportedTextData(ghost_zombie_idle0, png),
		ImportedTextData(ghost_zombie_idle1, png),
		ImportedTextData(ghost_zombie_side0, png),
		ImportedTextData(ghost_zombie_side1, png),
		ImportedTextData(ghost_zombie_side2, png),
		ImportedTextData(ghost_zombie_side3, png),
		ImportedTextData(ghost_zombie_up0, png),
		ImportedTextData(ghost_zombie_up1, png),
		ImportedTextData(ghost_zombie_up2, png),
		ImportedTextData(ghost_zombie_up3, png),

		ImportedTextData(mjzombie_death0, png),
		ImportedTextData(mjzombie_death1, png),
		ImportedTextData(mjzombie_death2, png),
		ImportedTextData(mjzombie_death3, png),
		ImportedTextData(mjzombie_down0, png),
		ImportedTextData(mjzombie_down1, png),
		ImportedTextData(mjzombie_down2, png),
		ImportedTextData(mjzombie_down3, png),
		ImportedTextData(mjzombie_idle0, png),
		ImportedTextData(mjzombie_idle1, png),
		ImportedTextData(mjzombie_side0, png),
		ImportedTextData(mjzombie_side1, png),
		ImportedTextData(mjzombie_side2, png),
		ImportedTextData(mjzombie_side3, png),
		ImportedTextData(mjzombie_up0, png),
		ImportedTextData(mjzombie_up1, png),
		ImportedTextData(mjzombie_up2, png),
		ImportedTextData(mjzombie_up3, png),

		ImportedTextData(banshee_idle0, png),
		ImportedTextData(banshee_idle1, png),
		ImportedTextData(banshee_death00, png),
		ImportedTextData(banshee_death01, png),
		ImportedTextData(banshee_death02, png),
		ImportedTextData(banshee_death03, png),
		ImportedTextData(banshee_death04, png),
		ImportedTextData(banshee_death05, png),
		ImportedTextData(banshee_death06, png),
		ImportedTextData(banshee_death07, png),
		ImportedTextData(banshee_death08, png),
		ImportedTextData(banshee_death09, png),
		ImportedTextData(banshee_death10, png),
		ImportedTextData(banshee_death11, png),

		ImportedTextData(player_classic_default_fireball, png),
		ImportedTextData(player_classic_default_fireball_particles_0, png),
		ImportedTextData(player_classic_default_fireball_particles_1, png),
		ImportedTextData(player_classic_default_fireball_particles_2, png),
		ImportedTextData(player_classic_default_fireball_particles_3, png),
		ImportedTextData(player_classic_default_fireball_particles_4, png),
		ImportedTextData(player_classic_default_fireball_particles_5, png),
		ImportedTextData(player_classic_default_fireball_particles_6, png),

		ImportedTextData(block_break00, png),
		ImportedTextData(block_break01, png),
		ImportedTextData(block_break02, png),
		ImportedTextData(block_break03, png),
		ImportedTextData(block_break04, png),
		ImportedTextData(block_break05, png),
		ImportedTextData(block_break06, png),
		ImportedTextData(block_break07, png),
		ImportedTextData(block_break08, png),
		ImportedTextData(block_break09, png),
		ImportedTextData(block_break10, png),
		ImportedTextData(block_break11, png),
		ImportedTextData(block_break12, png),
		ImportedTextData(block_break13, png),

		ImportedTextData(blood_splatter0, png),
		ImportedTextData(blood_splatter1, png),
		ImportedTextData(blood_splatter2, png),
		ImportedTextData(blood_splatter3, png),
		ImportedTextData(blood_splatter4, png),
		ImportedTextData(blood_splatter5, png),
		ImportedTextData(blood_splatter6, png),
		ImportedTextData(blood_splatter7, png),

		ImportedTextData(fire_explosion_0, png),
		ImportedTextData(fire_explosion_1, png),
		ImportedTextData(fire_explosion_2, png),
		ImportedTextData(fire_explosion_3, png),
		ImportedTextData(fire_explosion_4, png),
		ImportedTextData(fire_explosion_5, png),
		ImportedTextData(fire_explosion_6, png),

		ImportedTextData(nightmare_dead, png),
		ImportedTextData(nightmare_down1, png),
		ImportedTextData(nightmare_down2, png),
		ImportedTextData(nightmare_down3, png),
		ImportedTextData(nightmare_dying1, png),
		ImportedTextData(nightmare_dying2, png),
		ImportedTextData(nightmare_dying3, png),
		ImportedTextData(nightmare_dying4, png),
		ImportedTextData(nightmare_dying5, png),
		ImportedTextData(nightmare_dying6, png),
		ImportedTextData(nightmare_dying7, png),
		ImportedTextData(nightmare_dying8, png),
		ImportedTextData(nightmare_dying9, png),
		ImportedTextData(nightmare_dying10, png),
		ImportedTextData(nightmare_idle1, png),
		ImportedTextData(nightmare_idle2, png),
		ImportedTextData(nightmare_side1, png),
		ImportedTextData(nightmare_side2, png),
		ImportedTextData(nightmare_side3, png),
		ImportedTextData(nightmare_special, png),
		ImportedTextData(nightmare_up1, png),
		ImportedTextData(nightmare_up2, png),
		ImportedTextData(nightmare_up3, png),

		ImportedTextData(material_stone, png),
		ImportedTextData(material_stone_normal, png),
		ImportedTextData(healthbar_empty, png),
		ImportedTextData(gravestone_0, png),
		ImportedTextData(gravestone_1, png),
		ImportedTextData(torch_off, png),
		ImportedTextData(downwards_trapdoor, png),
		ImportedTextData(stone_stairs_down_nx, png),
		ImportedTextData(stone_stairs_up_nx, png),
		ImportedTextData(stone_stairs_down_py, png),
		ImportedTextData(stone_stairs_up_py, png),

		ImportedTextData(sprint0, png),
		ImportedTextData(sprint1, png),
		ImportedTextData(sprint2, png),

		ImportedTextData(dev_level_1_backdrop, png),
		ImportedTextData(dev_level_1_backdrop_normal, png),
		ImportedTextData(grass_backdrop, png),
		ImportedTextData(grass_backdrop_normal, png),
		ImportedTextData(snow_backdrop, png),
		ImportedTextData(dungeon_floor_backdrop, png),
		ImportedTextData(dungeon_floor_backdrop_normal, png),
	};

	tz::gl::image_resource load_image(TextureID texid)
	{
		std::string_view img_file_data = texture_data[static_cast<int>(texid)];
		return load_image_data(img_file_data);
	}

	tz::gl::image_resource load_image_data(std::string_view img_file_data)
	{
		int w, h, channels;
		stbi_uc* imgdata = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(img_file_data.data()), img_file_data.size(), &w, &h, &channels, 4);
		std::span<stbi_uc> imgdata_span{imgdata, static_cast<std::size_t>(w * h * 4)};
		tz::gl::image_resource ret = tz::gl::image_resource::from_memory(imgdata_span,
		{
			.format = tz::gl::image_format::RGBA32,
			.dimensions = static_cast<tz::vec2ui>(tz::vec2i{w, h}),
			.flags = {tz::gl::resource_flag::image_wrap_repeat}
		});
		stbi_image_free(imgdata);
		return ret;
	}

	TextureID get_normal_map_for(TextureID tex)
	{
		switch(tex)
		{
			case TextureID::Material_Stone:
				return TextureID::Material_Stone_Normal;
			break;
			case TextureID::Grass_Generic_Backdrop:
				return TextureID::Grass_Generic_Backdrop_Normal;
			break;
			case TextureID::Dungeon_Floor_Generic_Backdrop:
				return TextureID::Dungeon_Floor_Generic_Backdrop_Normal;
			break;
			case TextureID::DevLevel1_Backdrop:
				return TextureID::DevLevel1_Backdrop_Normal;
			break;
		}
		return TextureID::Smooth_Normals;
	}
}
