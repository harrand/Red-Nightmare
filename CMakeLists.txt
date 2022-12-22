cmake_minimum_required(VERSION 3.9)
project(RedNightmare
	DESCRIPTION "2D Power Fantasy Game"
	LANGUAGES CXX)

add_subdirectory(engine)
add_subdirectory(lib)

tz_add_executable(
	TARGET rednightmare
	SOURCES
		src/action.cpp
		src/action.hpp
		src/action.inl
		src/actor.cpp
		src/actor.hpp
		src/animation.cpp
		src/animation.hpp
		src/box.cpp
		src/box.hpp
		src/effect.cpp
		src/effect.hpp
		src/entity.hpp
		src/entity.inl
		src/event.hpp
		src/faction.hpp
		src/flag.hpp
		src/game.cpp
		src/game.hpp
		src/main.cpp
		src/images.cpp
		src/images.hpp
		src/level.cpp
		src/level.hpp
		src/scene.cpp
		src/scene.hpp
		src/stats.cpp
		src/stats.hpp
		src/quadtree.cpp
		src/quadtree.hpp
		src/quadtree.inl
		src/quad_renderer.cpp
		src/quad_renderer.hpp
		src/util.cpp
		src/util.hpp
)

add_shader(
	TARGET rednightmare
	SHADERS
	src/effects/empty.vertex.tzsl
	src/effects/empty.fragment.tzsl
	src/effects/rain.vertex.tzsl
	src/effects/rain.fragment.tzsl
	src/default.vertex.tzsl
	src/default.fragment.tzsl
	src/quad.vertex.tzsl
	src/quad.fragment.tzsl
)

target_link_libraries(rednightmare PRIVATE stb)

add_text(
	TARGET rednightmare
	INPUT_DIR ${PROJECT_SOURCE_DIR}
	OUTPUT_DIR ${PROJECT_BINARY_DIR}
	TEXT_FILES
		res/images/missing.png
		res/images/invisible.png
		res/images/player/akhara/skins/classic/player_classic_dead.png
		res/images/player/akhara/skins/classic/player_classic_down1.png
		res/images/player/akhara/skins/classic/player_classic_down2.png
		res/images/player/akhara/skins/classic/player_classic_down3.png
		res/images/player/akhara/skins/classic/player_classic_dying1.png
		res/images/player/akhara/skins/classic/player_classic_dying2.png
		res/images/player/akhara/skins/classic/player_classic_dying3.png
		res/images/player/akhara/skins/classic/player_classic_dying4.png
		res/images/player/akhara/skins/classic/player_classic_dying5.png
		res/images/player/akhara/skins/classic/player_classic_dying6.png
		res/images/player/akhara/skins/classic/player_classic_dying7.png
		res/images/player/akhara/skins/classic/player_classic_dying8.png
		res/images/player/akhara/skins/classic/player_classic_idle1.png
		res/images/player/akhara/skins/classic/player_classic_idle2.png
		res/images/player/akhara/skins/classic/player_classic_side1.png
		res/images/player/akhara/skins/classic/player_classic_side2.png
		res/images/player/akhara/skins/classic/player_classic_side3.png
		res/images/player/akhara/skins/classic/player_classic_special.png
		res/images/player/akhara/skins/classic/player_classic_up1.png
		res/images/player/akhara/skins/classic/player_classic_up2.png
		res/images/player/akhara/skins/classic/player_classic_up3.png

		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_cast0.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_cast1.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_cast2.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_cast3.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_cast4.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_cast5.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_cast6.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_cast7.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_death00.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_death01.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_death02.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_death03.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_death04.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_death05.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_death06.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_death07.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_death08.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_death09.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_death10.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_death11.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_down0.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_down1.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_down2.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_down3.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_endcast0.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_endcast1.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_endcast2.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_endcast3.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_endcast4.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_endcast5.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_idle0.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_idle1.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_side0.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_side1.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_side2.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_side3.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_up0.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_up1.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_up2.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_up3.png

		res/images/enemies/ghosts/zombie/ghost_zombie_death0.png
		res/images/enemies/ghosts/zombie/ghost_zombie_death1.png
		res/images/enemies/ghosts/zombie/ghost_zombie_death2.png
		res/images/enemies/ghosts/zombie/ghost_zombie_death3.png
		res/images/enemies/ghosts/zombie/ghost_zombie_down0.png
		res/images/enemies/ghosts/zombie/ghost_zombie_down1.png
		res/images/enemies/ghosts/zombie/ghost_zombie_down2.png
		res/images/enemies/ghosts/zombie/ghost_zombie_down3.png
		res/images/enemies/ghosts/zombie/ghost_zombie_idle0.png
		res/images/enemies/ghosts/zombie/ghost_zombie_idle1.png
		res/images/enemies/ghosts/zombie/ghost_zombie_side0.png
		res/images/enemies/ghosts/zombie/ghost_zombie_side1.png
		res/images/enemies/ghosts/zombie/ghost_zombie_side2.png
		res/images/enemies/ghosts/zombie/ghost_zombie_side3.png
		res/images/enemies/ghosts/zombie/ghost_zombie_up0.png
		res/images/enemies/ghosts/zombie/ghost_zombie_up1.png
		res/images/enemies/ghosts/zombie/ghost_zombie_up2.png
		res/images/enemies/ghosts/zombie/ghost_zombie_up3.png

		res/images/enemies/ghosts/mjzombie/mjzombie_death0.png
		res/images/enemies/ghosts/mjzombie/mjzombie_death1.png
		res/images/enemies/ghosts/mjzombie/mjzombie_death2.png
		res/images/enemies/ghosts/mjzombie/mjzombie_death3.png
		res/images/enemies/ghosts/mjzombie/mjzombie_down0.png
		res/images/enemies/ghosts/mjzombie/mjzombie_down1.png
		res/images/enemies/ghosts/mjzombie/mjzombie_down2.png
		res/images/enemies/ghosts/mjzombie/mjzombie_down3.png
		res/images/enemies/ghosts/mjzombie/mjzombie_idle0.png
		res/images/enemies/ghosts/mjzombie/mjzombie_idle1.png
		res/images/enemies/ghosts/mjzombie/mjzombie_side0.png
		res/images/enemies/ghosts/mjzombie/mjzombie_side1.png
		res/images/enemies/ghosts/mjzombie/mjzombie_side2.png
		res/images/enemies/ghosts/mjzombie/mjzombie_side3.png
		res/images/enemies/ghosts/mjzombie/mjzombie_up0.png
		res/images/enemies/ghosts/mjzombie/mjzombie_up1.png
		res/images/enemies/ghosts/mjzombie/mjzombie_up2.png
		res/images/enemies/ghosts/mjzombie/mjzombie_up3.png

		res/images/enemies/ghosts/banshee/banshee_idle0.png
		res/images/enemies/ghosts/banshee/banshee_idle1.png
		res/images/enemies/ghosts/banshee/banshee_death00.png
		res/images/enemies/ghosts/banshee/banshee_death01.png
		res/images/enemies/ghosts/banshee/banshee_death02.png
		res/images/enemies/ghosts/banshee/banshee_death03.png
		res/images/enemies/ghosts/banshee/banshee_death04.png
		res/images/enemies/ghosts/banshee/banshee_death05.png
		res/images/enemies/ghosts/banshee/banshee_death06.png
		res/images/enemies/ghosts/banshee/banshee_death07.png
		res/images/enemies/ghosts/banshee/banshee_death08.png
		res/images/enemies/ghosts/banshee/banshee_death09.png
		res/images/enemies/ghosts/banshee/banshee_death10.png
		res/images/enemies/ghosts/banshee/banshee_death11.png

		res/images/player/akhara/ability/player_classic_default_fireball.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_0.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_1.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_2.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_3.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_4.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_5.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_6.png

		res/images/effects/block_break/block_break00.png
		res/images/effects/block_break/block_break01.png
		res/images/effects/block_break/block_break02.png
		res/images/effects/block_break/block_break03.png
		res/images/effects/block_break/block_break04.png
		res/images/effects/block_break/block_break05.png
		res/images/effects/block_break/block_break06.png
		res/images/effects/block_break/block_break07.png
		res/images/effects/block_break/block_break08.png
		res/images/effects/block_break/block_break09.png
		res/images/effects/block_break/block_break10.png
		res/images/effects/block_break/block_break11.png
		res/images/effects/block_break/block_break12.png
		res/images/effects/block_break/block_break13.png

		res/images/effects/blood_splatter/blood_splatter0.png
		res/images/effects/blood_splatter/blood_splatter1.png
		res/images/effects/blood_splatter/blood_splatter2.png
		res/images/effects/blood_splatter/blood_splatter3.png
		res/images/effects/blood_splatter/blood_splatter4.png
		res/images/effects/blood_splatter/blood_splatter5.png
		res/images/effects/blood_splatter/blood_splatter6.png
		res/images/effects/blood_splatter/blood_splatter7.png

		res/images/effects/fire_explosion/fire_explosion_0.png
		res/images/effects/fire_explosion/fire_explosion_1.png
		res/images/effects/fire_explosion/fire_explosion_2.png
		res/images/effects/fire_explosion/fire_explosion_3.png
		res/images/effects/fire_explosion/fire_explosion_4.png
		res/images/effects/fire_explosion/fire_explosion_5.png
		res/images/effects/fire_explosion/fire_explosion_6.png

		res/images/enemies/nightmare/nightmare_dead.png
		res/images/enemies/nightmare/nightmare_down1.png
		res/images/enemies/nightmare/nightmare_down2.png
		res/images/enemies/nightmare/nightmare_down3.png
		res/images/enemies/nightmare/nightmare_dying1.png
		res/images/enemies/nightmare/nightmare_dying2.png
		res/images/enemies/nightmare/nightmare_dying3.png
		res/images/enemies/nightmare/nightmare_dying4.png
		res/images/enemies/nightmare/nightmare_dying5.png
		res/images/enemies/nightmare/nightmare_dying6.png
		res/images/enemies/nightmare/nightmare_dying7.png
		res/images/enemies/nightmare/nightmare_dying8.png
		res/images/enemies/nightmare/nightmare_dying9.png
		res/images/enemies/nightmare/nightmare_dying10.png
		res/images/enemies/nightmare/nightmare_idle1.png
		res/images/enemies/nightmare/nightmare_idle2.png
		res/images/enemies/nightmare/nightmare_side1.png
		res/images/enemies/nightmare/nightmare_side2.png
		res/images/enemies/nightmare/nightmare_side3.png
		res/images/enemies/nightmare/nightmare_special.png
		res/images/enemies/nightmare/nightmare_up1.png
		res/images/enemies/nightmare/nightmare_up2.png
		res/images/enemies/nightmare/nightmare_up3.png

		res/images/materials/material_stone.png

		res/images/powerups/sprint/sprint0.png
		res/images/powerups/sprint/sprint1.png
		res/images/powerups/sprint/sprint2.png

		res/images/levels/dev_level_0.png
		res/images/levels/dev_level_1.png
		res/images/levels/dev_level_1_backdrop.png
		res/images/levels/dev_level_2.png
		res/images/levels/generic/grass_backdrop.png
		res/images/levels/generic/dungeon_floor_backdrop.png
)
