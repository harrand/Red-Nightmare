cmake_minimum_required(VERSION 3.9)
project(RedNightmare
	DESCRIPTION "2D Power Fantasy Game"
	LANGUAGES CXX)

add_subdirectory(engine)
add_subdirectory(lib)

add_app(
	TARGET rednightmare
	SOURCE_FILES
		src/action.cpp
		src/action.hpp
		src/action.inl
		src/actor.cpp
		src/actor.hpp
		src/animation.cpp
		src/animation.hpp
		src/faction.hpp
		src/main.cpp
		src/images.cpp
		src/images.hpp
		src/scene.cpp
		src/scene.hpp
		src/stats.cpp
		src/stats.hpp
		src/quad_renderer.cpp
		src/quad_renderer.hpp
		src/util.cpp
		src/util.hpp
	SHADER_SOURCES
		src/default.vertex.tzsl
		src/default.fragment.tzsl
		src/quad.vertex.tzsl
		src/quad.fragment.tzsl
	CUSTOM_ICON
		${PROJECT_SOURCE_DIR}/rednightmare.ico
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

		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_down0.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_down1.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_down2.png
		res/images/player/akhara/skins/classic_lowpoly/player_classic_lowpoly_down3.png
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

		res/images/player/akhara/ability/player_classic_default_fireball.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_0.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_1.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_2.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_3.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_4.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_5.png
		res/images/player/akhara/ability/player_classic_default_fireball_particles_6.png

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
)
