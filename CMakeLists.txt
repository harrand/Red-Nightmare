cmake_minimum_required(VERSION 3.9)
project(RedNightmare
	DESCRIPTION "2D Power Fantasy Game"
	LANGUAGES CXX)
add_subdirectory(engine)

add_app(
	TARGET rednightmare
	SOURCE_FILES
		src/main.cpp
	SHADER_SOURCES
		src/default.vertex.tzsl
		src/default.fragment.tzsl
	CUSTOM_ICON
		${PROJECT_SOURCE_DIR}/rednightmare.ico
)
