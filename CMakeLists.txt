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
	TARGET librn
	SOURCES
		src/gamelib/core/entity.cpp
		src/gamelib/core/entity.hpp
		src/gamelib/core/entity.inl

		src/gamelib/render/quad_renderer.cpp
		src/gamelib/render/quad_renderer.hpp

		src/gamelib/game.cpp
		src/gamelib/game.hpp
		src/gamelib/version.cpp
		src/gamelib/version.hpp
)
target_include_directories(librn PUBLIC ${PROJECT_SOURCE_DIR}/src)

add_shader(
	TARGET librn
	SHADERS
	src/gamelib/render/shaders/empty.vertex.tzsl
	src/gamelib/render/shaders/empty.fragment.tzsl
	src/gamelib/render/shaders/quad.vertex.tzsl
	src/gamelib/render/shaders/quad.fragment.tzsl
)

target_link_libraries(librn PUBLIC stb)
target_compile_definitions(librn PUBLIC -DGAME_VERSION=0x${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH}${PROJECT_VERSION_TWEAK})

tz_add_executable(
	TARGET rn
	SOURCES
		src/main.cpp
)

target_link_libraries(rn PRIVATE librn stb)

#add_text(
#	TARGET librn
#	INPUT_DIR ${PROJECT_SOURCE_DIR}
#	OUTPUT_DIR ${PROJECT_BINARY_DIR}
#	TEXT_FILES
#		res/images/levels/backdrops/generic/dungeon_floor_backdrop_normal.png
#)
