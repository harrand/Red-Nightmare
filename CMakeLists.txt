cmake_minimum_required(VERSION 3.9)
project(RedNightmare
	DESCRIPTION "2D Power Fantasy Game"
	LANGUAGES CXX)

add_subdirectory(engine)
add_subdirectory(lib)

add_app(
	TARGET rednightmare
	SOURCE_FILES
		src/main.cpp
		src/images.cpp
		src/images.hpp
		src/quad_renderer.cpp
		src/quad_renderer.hpp
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
		res/images/player/classic/idle1.png
		res/images/player/classic/idle2.png
)
