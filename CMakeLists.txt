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
		src/gamelib/messaging/scene.cpp
		src/gamelib/messaging/scene.hpp
		src/gamelib/messaging/system.hpp

		src/gamelib/audio.cpp
		src/gamelib/audio.hpp
		src/gamelib/rnlib.cpp
		src/gamelib/rnlib.hpp
		src/gamelib/scene.cpp
		src/gamelib/scene.hpp
		src/gamelib/entity.cpp
		src/gamelib/entity.hpp
		src/gamelib/config.hpp
)
#add_shader(
#	TARGET rednightmare_gamelib
#	SHADERS
#		src/gamelib/renderer/shaders/scene_renderer.fragment.tzsl	
#		src/gamelib/renderer/shaders/pixelate.vertex.tzsl	
#		src/gamelib/renderer/shaders/pixelate.fragment.tzsl	
#)
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

target_link_libraries(rednightmare PRIVATE rednightmare_gamelib)

add_text(
	TARGET rednightmare_gamelib
	INPUT_DIR ${PROJECT_SOURCE_DIR}
	OUTPUT_DIR ${PROJECT_BINARY_DIR}
	TEXT_FILES
		res/fonts/ProggyClean.ttf
		res/models/human_animated_textured.glb
		res/models/plane.glb
)
