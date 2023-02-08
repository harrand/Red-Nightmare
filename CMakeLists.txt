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
	TARGET rnlib
	SOURCES
		src/gamelib/core/entity.cpp
		src/gamelib/core/entity.hpp
		src/gamelib/core/entity.inl
		src/gamelib/core/mount.hpp
		src/gamelib/core/transform.cpp
		src/gamelib/core/transform.hpp

		src/gamelib/gameplay/actor/components/animation.hpp
		src/gamelib/gameplay/actor/components/keyboard_control.hpp
		src/gamelib/gameplay/actor/components/sprite.hpp
		src/gamelib/gameplay/actor/entity.cpp
		src/gamelib/gameplay/actor/entity.hpp
		src/gamelib/gameplay/actor/system.cpp
		src/gamelib/gameplay/actor/system.hpp

		src/gamelib/render/animation.cpp
		src/gamelib/render/animation.hpp
		src/gamelib/render/camera.cpp
		src/gamelib/render/camera.hpp
		src/gamelib/render/image.cpp
		src/gamelib/render/image.hpp
		src/gamelib/render/quad_renderer.cpp
		src/gamelib/render/quad_renderer.hpp

		src/gamelib/game.cpp
		src/gamelib/game.hpp
		src/gamelib/version.cpp
		src/gamelib/version.hpp
)
target_include_directories(rnlib PUBLIC ${PROJECT_SOURCE_DIR}/src)

add_shader(
	TARGET rnlib
	SHADERS
	src/gamelib/render/shaders/empty.vertex.tzsl
	src/gamelib/render/shaders/empty.fragment.tzsl
	src/gamelib/render/shaders/quad.vertex.tzsl
	src/gamelib/render/shaders/quad.fragment.tzsl
)

target_link_libraries(rnlib PUBLIC stb)
target_compile_definitions(rnlib PUBLIC -DGAME_VERSION=0x${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH}${PROJECT_VERSION_TWEAK})

add_text(
	TARGET rnlib
	INPUT_DIR ${PROJECT_SOURCE_DIR}/res/assets/
	OUTPUT_DIR ${PROJECT_BINARY_DIR}/res/assets/
	TEXT_FILES
		images/meta/undefined.png
		images/meta/invisible.png
		images/races/human/base/human_base_idle0.png
		images/races/human/base/human_base_idle1.png
		images/races/human/base/human_base_side0.png
		images/races/human/base/human_base_side1.png
		images/races/human/base/human_base_side2.png
		images/races/human/base/human_base_side3.png
		images/races/human/hair/long/human_hair_long_idle0.png
		images/races/human/hair/long/human_hair_long_idle1.png
		images/races/human/hair/long/human_hair_long_side0.png
		images/races/human/hair/long/human_hair_long_side1.png
		images/races/human/hair/long/human_hair_long_side2.png
		images/races/human/hair/long/human_hair_long_side3.png
		images/races/human/eyes/dot/human_eyes_dot_idle0.png
		images/races/human/eyes/dot/human_eyes_dot_idle1.png
		images/races/human/eyes/dot/human_eyes_dot_side0.png
		images/races/human/eyes/dot/human_eyes_dot_side1.png
		images/races/human/eyes/dot/human_eyes_dot_side2.png
		images/races/human/eyes/dot/human_eyes_dot_side3.png
		images/races/human/equipment/feet/dot/human_equipment_feet_dot_idle0.png
		images/races/human/equipment/feet/dot/human_equipment_feet_dot_side0.png
		images/races/human/equipment/feet/dot/human_equipment_feet_dot_side1.png
		images/races/human/equipment/feet/dot/human_equipment_feet_dot_side2.png
		images/races/human/equipment/feet/dot/human_equipment_feet_dot_side3.png
)

tz_add_executable(
	TARGET rn
	SOURCES
		src/main.cpp
)

target_link_libraries(rn PRIVATE rnlib stb)
