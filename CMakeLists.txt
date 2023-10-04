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
		src/gamelib/rnlib.cpp
		src/gamelib/rnlib.hpp
)
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

#add_text(
#	TARGET rednightmare_gamelib
#	INPUT_DIR ${PROJECT_SOURCE_DIR}
#	OUTPUT_DIR ${PROJECT_BINARY_DIR}
#	TEXT_FILES
#		res/images/missing.png
#)
