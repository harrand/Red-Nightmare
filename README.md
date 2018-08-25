# Asteroids (Topaz)
## About

Asteroids (Topaz) is a modern reimplementation of the Atari Asteroids arcade game. This third-party implementation of Asteroids runs on PC, Mac and Linux using the Topaz engine.

I have created this reimplementation twice. There is a Unity implementation aswell as this Topaz implementation. The Unity version can be found [here](https://github.com/Harrand/Asteroids).

### Built With

* [Topaz](https://github.com/Harrand/Topaz) - The game engine used to create the game.

### License

This project is licensed under the Apache License 2.0 - see the [LICENSE.md](LICENSE.md) file for details.

## Installation
### Prerequisites

* OpenGL v4.3 or later. See your graphics card information to verify that you support this.
* SDL2-supported hardware. See [their wiki on installation](http://wiki.libsdl.org/Installation) to verify this.
### Compilation

If you have CMake:

See `CMakeLists.txt` in the root directory.

If you do not, see the windows batch files (.bat) included in the root folder to aid with compilation. If you are not on Windows nor currently have CMake, then you must [install CMake](https://cmake.org/) to use this library.