# Dodge Graphics Library

## About

*Dodge Graphics Library* (DGL) is a graphics library designed for 2D gaming applications. Based on [Raylib](https://github.com/raysan5/raylib) (by Ramon Santamaria) the library is written entirely in C.

Have you ever wanted to write a graphics program or a small 2D video game in C, but couldn't wrap your head around the specifics (like sprites, collision, etc)? Don't worry! DGL has got you covered. DGL abstracts some of the technical details of game development, allowing you to focus on writing the game itself.

DGL implements:

- Sprite handling
- Pixmap creation (see footnote 1)
- Collision detection (rectangle model)
- Compilation

DGL's simplicity will allow you to make fun games without spending time learning and programming the intricacies of game development.

Of course, DGL is young and imperfect. Please, help us out with tips, suggestions, and bug reports!

If you write a game using DGL, please send a screenshot and tell us about it! The screenshot may even get a place on the GitHub Blog!

### contact

- Email us at [serpaeos.devers@gmail.com](mailto:serpaeos.devers@gmail.com)
- Check out my [Fiverr](https://www.fiverr.com/s/qpkPr9) and [LinkedIn](www.linkedin.com/in/jaihsonkresak) pages
- [Buy me a Pizza :)](https://www.buymeacoffee.com/serpaeosdel)

## Set-up

### Requirements and dependencies

DGL is dependant on [Raylib](https://github.com/raysan5/raylib). Please *install Raylib as a **static library*** first.

> NOTE: Raylib is currently only considered stable for the X11 windowing system. Inherently, DGL is also only considered stable for X11. However, in practice, Raylib/DGL games built for X11 seem to usually run fine on Wayland. Also, the XWayland interface can broaden a Wayland users support. <br> In short, don't worry if you're on Wayland, everything will (probably) be okay! Please let us know how your experience goes if you're developing and running DGL applications on Wayland.

To build you will need Make and a C compiler (GCC preferably, although Clang works perfectly fine).

To install make and gcc:

#### Debian based (Ubuntu, Mint, etc)

```sudo apt install make build-essential```

#### RHEL based (Fedora, CentOS, etc)

```sudo dnf install make build-essential```

#### Windows

DGL is not currently available on Windows. Please contact us to help port to project to Windows!

## Install

To build and install DGL, simply run

```sh
make
```

If you wish to update DGL, download the new source code and run

```sh
make
```

To uninstall DGL, use

```sh
make uninstall
```

Note that DGL is available as a library (recommended) and an application. In the latter case, the developer would have to integrate their own program into `./src/main.c` and add any other source files to the `src` variable in the Makefile. To compile as an application and not a library, use

```sh
make clean
make freestanding
```

DGL compiles with the default C compiler on your system. If you with to force a different program as your C compiler, use the `CC` option. For example:

```sh
make CC=clang
```

If you wish to send flags to the compiler, use the `FLAGS` option. For example:

```sh
make FLAGS="-g -std=c2x"
```

## Developing applications

Developing DGL applications is easy! You will need both a source file and a DGL session file.

### Building

Use the following to build a DGL application:

```sh
cc source1.c source2.c -lDGL -lGL -lm -lpthread -ldl -lrt -lX11 -o program
```

### DGL session file

A DGL application will need a session file to provide information relevant for your application, such as window dimensions and window title. The session file can have any name or extension, and its path relative to the executable will be provided in the source file. A sample DGL session file would look like this:

```plain-text
windowWidth:1000
windowHeight:500
title:My DGL application
```

Note that:

- there are no spaces between the key (`windowWidth`) and the value (`1000`). They are only separated by one colon.
- keys are case sensitive (`windowWidth` is not the same as `Windowwidth`).
- if information is lacking in the session file, or the provided information is not valid, the defaults will take effect. The default values are:

```plain-text
windowWidth:800
windowHeight:500
title:Dodge Gaming Library
```

### Source file

The source file should take the following form:

```c
#include <dgl.h>
...

int main(int argc, char **argv)
{
    start("DGLsession.dat") // path is relative to executable

    ... (see programming manual)

    exit(0); //end the program with exit(), not return. 
             //This will ensure resources are deallocated by DGL
}
```

The `start` function initiates DGL and supplies the relative file path to the DGL session file.

The program should end will an `exit()` function call, not a return. This will ensure an `atexit` callback set up by DGL to deallocate resources will run.

For more information, see the [dgl.h](./dgl.h) header file, as well as the [programming manual](./documentation/TUTORIAL.md).

## How does DGL work?

DGL works by organizing sprites (or objects) into levels and layers, rendering everything into the *final image* and displaying this final image to to screen.

### Sprites

Sprites are objects that are seen on screen. In the context of the DGL, everything is a sprite, from to background to the text to an object.

### Layers and Levels

Layers can be thought of as 2D planes on which sprites can exist and through which they can interact.

Layers provide a good way to organize sprites. The developer can test for collision with only a certain group of sprites.

When DGL renders a layer, it puts the first sprite in the layer on the bottom and the last on top, such that if the first and the last sprites are rendered in the same location, the last sprite will appear to be on top the first sprite. This order of precedence is referred to by the term levels. Sprites on a lower level are rendered *below* sprites of a higher level. These levels are still within the same layer.

### Final image

After all layers are rendered to their respective caches, they are compiled into the *final image*. The final image can be thought of as the "ultimate layer." The final image is what is drawn to the screen for the user to see.

The final image is compiled from all the layers. The layers are stacked "on top" of each other in a similar manner as levels are stacked in a layer; sprites on layer 0, for example, appear to be *below* sprites on layer 1 should they be drawn in the same location.

-----

## Definitions

[1]: A pixmap is an array of pixel data that represents an image. Each element in the array tells the colour of the corresponding pixel. To create your own sprites, see the project [Pixdraw](https://github.com/JaihsonK/pixdraw)
