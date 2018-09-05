# Red Nightmare
## About

I have created this reimplementation twice. There is a university coursework edition aswell as the Topaz implementation. The coursework edition uses an unlicensed library, but the game itself can be found [here](https://github.com/Harrand/Red-Nightmare-Coursework-Edition).

### Story

Red Nightmare is a unique 2D game. You play as a powerful wizard stranded in a monster-infested forest. You are not being preyed upon by these monsters however; it is they who are preyed upon by ***you***. Luckily, you are a powerful wizard, armed with telekinesis and a magic fireball companion.

You can use your telekinetic magic to drag the monsters into harms way. Anything that gets too close to your fireball will be incinerated, including yourself. You can also command your fireball to move to a target location; either to protect yourself from invading monsters or to move into groups of monsters to demolish them.

Beware, however; a presence lingers in the forest, causing it to decay and die. It is a powerful presence indeed, but not your own. Can you survive the nightmare?

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