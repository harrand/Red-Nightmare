cmake_minimum_required(VERSION 3.9)
project(RedNightmare)

set(CMAKE_CXX_STANDARD 17)
set(ASTEROIDS_COMPILE_FLAGS "-Wall -Wextra -pedantic-errors -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASTEROIDS_COMPILE_FLAGS}")

include_directories(RedNightmare ${PROJECT_SOURCE_DIR}/inc)
include_directories(RedNightmare ${PROJECT_SOURCE_DIR}/src)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(RedNightmare
        src/main.cpp src/player.cpp src/player.hpp src/sprite_collection.cpp src/sprite_collection.hpp src/entity_manager.cpp src/entity_manager.hpp src/audio_manager.cpp src/audio_manager.hpp src/game_sprite.cpp src/game_sprite.hpp src/entity.cpp src/entity.hpp src/enemies/ghost.cpp src/enemies/ghost.hpp src/cursor.cpp src/cursor.hpp src/orbs/fireball.cpp src/orbs/fireball.hpp src/orbs/frostball.cpp src/orbs/frostball.hpp src/orbs/blackball.cpp src/orbs/blackball.hpp src/enemies/nightmare.cpp src/enemies/nightmare.hpp)
target_link_libraries(RedNightmare OpenGL32 SDL2 SDL2_ttf topaz)