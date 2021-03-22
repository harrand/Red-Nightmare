cmake_minimum_required(VERSION 3.9)
project(RedNightmare)

set(CMAKE_CXX_STANDARD 17)
set(ASTEROIDS_COMPILE_FLAGS "-Wall -Wextra -pedantic-errors -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASTEROIDS_COMPILE_FLAGS}")

add_executable(RedNightmare
        src/main.cpp
        
        src/sprite_manager.cpp
        src/sprite_manager.inl
        src/sprite_manager.hpp
        src/sprite.cpp
        src/sprite.hpp)
        add_dependencies(RedNightmare topaz)
target_link_libraries(RedNightmare PRIVATE topaz)

add_subdirectory(lib)