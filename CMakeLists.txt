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
	 	src/gamelib/entity/api.cpp
	 	src/gamelib/entity/api.hpp
		src/gamelib/entity/scene.cpp
		src/gamelib/entity/scene.hpp

		src/gamelib/logic/stats.cpp
		src/gamelib/logic/stats.hpp

		src/gamelib/physics/aabb.cpp
		src/gamelib/physics/aabb.hpp
		src/gamelib/physics/quadtree.cpp
		src/gamelib/physics/quadtree.hpp

		src/gamelib/renderer/scene_renderer.cpp
		src/gamelib/renderer/scene_renderer.hpp
		src/gamelib/renderer/texture_manager.cpp
		src/gamelib/renderer/texture_manager.hpp

		src/gamelib/rnlib.cpp
		src/gamelib/rnlib.hpp
		src/gamelib/config.hpp
)
add_shader(
	TARGET rednightmare_gamelib
	SHADERS
		src/gamelib/renderer/shaders/scene_renderer.fragment.tzsl	
		src/gamelib/renderer/shaders/pixelate.vertex.tzsl	
		src/gamelib/renderer/shaders/pixelate.fragment.tzsl	
)
target_link_libraries(rednightmare_gamelib PUBLIC shared_recursive_mutex)
target_include_directories(rednightmare_gamelib PUBLIC ${PROJECT_SOURCE_DIR}/src)

#add_shader(
#	TARGET rednightmare_gamelib
#	SHADERS
#	src/gfx/effects/empty.vertex.tzsl
#)

target_compile_definitions(rednightmare_gamelib PUBLIC -DGAME_VERSION=0x${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH}${PROJECT_VERSION_TWEAK})

tz_add_executable(
	TARGET rednightmare
	SOURCES
		src/game/main.cpp
)

target_link_libraries(rednightmare PRIVATE rednightmare_gamelib)

add_text(
	TARGET rednightmare_gamelib
	INPUT_DIR ${PROJECT_SOURCE_DIR}
	OUTPUT_DIR ${PROJECT_BINARY_DIR}
	TEXT_FILES
		res/models/human_animated_textured.glb
		res/models/plane.glb
		src/gamelib/lua/ability.lua
		src/gamelib/lua/combat.lua
		src/gamelib/lua/director.lua
		src/gamelib/lua/entity.lua
		src/gamelib/lua/equipment.lua
		src/gamelib/lua/item.lua
		src/gamelib/lua/level.lua
)
