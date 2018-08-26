cmake_minimum_required(VERSION 3.9)
project(TopazAsteroids)

set(CMAKE_CXX_STANDARD 17)
set(ASTEROIDS_COMPILE_FLAGS "-Wall -Wextra -pedantic-errors -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASTEROIDS_COMPILE_FLAGS}")

include_directories(TopazAsteroids ${PROJECT_SOURCE_DIR}/inc)
include_directories(TopazAsteroids ${PROJECT_SOURCE_DIR}/src)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(TopazAsteroids
        src/main.cpp src/player.cpp src/player.hpp src/sprite_collection.cpp src/sprite_collection.hpp src/asteroid.cpp src/asteroid.hpp src/entity_manager.cpp src/entity_manager.hpp src/bullet.cpp src/bullet.hpp src/audio_manager.cpp src/audio_manager.hpp src/entity_spawner.cpp src/entity_spawner.hpp)
target_link_libraries(TopazAsteroids OpenGL32 SDL2 SDL2_ttf topaz)