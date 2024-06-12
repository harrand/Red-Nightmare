cmake_minimum_required(VERSION 3.9)

configure_file(version.txt version_txt_dummy.xml)
file(READ version.txt GAME_VERSION_TXT)
string(REGEX MATCH "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" GAME_VERSION ${GAME_VERSION_TXT})

project(RedNightmare
	DESCRIPTION "2D Action Roguelike"
	LANGUAGES CXX
	VERSION ${GAME_VERSION})

add_subdirectory(engine)
add_subdirectory(lib)

tz_add_library(
	TARGET rednightmare_gamelib
	SOURCES
		src/gamelib/input/kbm.cpp
		src/gamelib/input/kbm.hpp

		src/gamelib/lua/meta.cpp
		src/gamelib/lua/meta.hpp

		src/gamelib/messaging/scene.cpp
		src/gamelib/messaging/scene.hpp
		src/gamelib/messaging/system.hpp
		
		src/gamelib/physics/aabb.cpp
		src/gamelib/physics/aabb.hpp
		src/gamelib/physics/grid_hierarchy.cpp
		src/gamelib/physics/grid_hierarchy.hpp

		src/gamelib/render/scene_renderer.cpp
		src/gamelib/render/scene_renderer.hpp

		src/gamelib/audio.cpp
		src/gamelib/audio.hpp
		src/gamelib/rnlib.cpp
		src/gamelib/rnlib.hpp
		src/gamelib/scene.cpp
		src/gamelib/scene.dbgui.cpp
		src/gamelib/scene.hpp
		src/gamelib/entity.cpp
		src/gamelib/entity.hpp
		src/gamelib/config.hpp
)
add_shader(
	TARGET rednightmare_gamelib
	SHADERS
		src/gamelib/render/shaders/scene_renderer.fragment.tzsl	
		src/gamelib/render/shaders/pixelate.fragment.tzsl	
		src/gamelib/render/shaders/deferred.fragment.tzsl	
		src/gamelib/render/shaders/fullscreen_triangle.vertex.tzsl	
)
target_include_directories(rednightmare_gamelib PUBLIC ${PROJECT_SOURCE_DIR}/src)

#add_shader(
#	TARGET rednightmare_gamelib
#	SHADERS
#	src/gfx/effects/empty.vertex.tzsl
#)

target_compile_definitions(rednightmare_gamelib PUBLIC -DGAME_VERSION=0x${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH}${PROJECT_VERSION_TWEAK})
target_link_libraries(rednightmare_gamelib PUBLIC soloud)

tz_add_executable(
	TARGET rednightmare
	SOURCES
		src/game/main.cpp
)

# TODO: Do this whenever any of the mods directory changes. Right now, this only updates/runs if rednightmare.exe is rebuilt. Otherwise they stay out-of-sync.
# Copy mods directory to wherever rednightmare.exe is put.
add_custom_command(TARGET rednightmare POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/mods $<TARGET_FILE_DIR:rednightmare>/mods/
)

target_link_libraries(rednightmare PRIVATE rednightmare_gamelib)

add_text(
	TARGET rednightmare_gamelib
	INPUT_DIR ${PROJECT_SOURCE_DIR}
	OUTPUT_DIR ${PROJECT_BINARY_DIR}
	TEXT_FILES
		res/fonts/ProggyClean.ttf
		src/gamelib/lua/buff.lua
		src/gamelib/lua/entity.lua
		src/gamelib/lua/game.lua
		src/gamelib/lua/item.lua
		src/gamelib/lua/level.lua
		src/gamelib/lua/mod.lua
		src/gamelib/lua/player.lua
		src/gamelib/lua/spell.lua
		src/gamelib/lua/util.lua
)
