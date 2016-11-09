# GrSim - INSTALL
## Overview

We developed grSim on Ubuntu 9.10+ OS. (Ubuntu 14.04+ tested and is recommended). It is very important that the Graphics Card Driver is installed. (Ubuntu packages of nVidia and AMD(ATI) graphics cards are available). grSim will compile and run in both 32 and 64 bits Linux and Mac-osx.

GrSim is written in C++, in order to compile it, you will need a working toolchain and a c++ compiler.

## Dependencies

GrSim depends on:

- OpenGL <https://www.opengl.org>
- Qt4 Development Libraries <https://www.qt.io>
- Open Dynamics Engine (ODE) <http://www.ode.org>
- VarTypes Library <https://github.com/szi/vartypes>
- Google Protobuf <https://github.com/google/protobuf>

We use CMake <http://www.cmake.org/> to build and install GrSim.


> It's neccessery to use ode double percision so if you'r using source code, please attention to use `./configure --enable-double-precision` for configuration.

## Linux/Unix Installation

If you run a Debian system, or derivative, first ensure that these dependencies are there:

```bash
sudo apt-get install build-essential cmake libqt4-dev libgl1-mesa-dev libglu1-mesa-dev libprotobuf-dev protobuf-compiler libode-dev
```

Next we need to install VarTypes manually, which itself depends on Qt4 so please make sure that you installed Qt4 first :

    $ (Get VarTypes)
    $ cd ./vartypes
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ sudo make install

Next, pull down the project:

    $ git clone https://github.com/mani-monaj/grSim.git
    $ cd ./grSim

Create a build directory within the project (this is ignored by .gitignore):

    $ mkdir build
    $ cd build

Run CMake to generate the makefiles:

    $ cmake ..

Then compile the program:

    $ make

The program is now under ../bin directory with the name grSim. You can run it from here just by typing "../bin/grSim"

## Mac OS X Installation

To make it easier to install the dependencies, you can use a package manager like Homebrew.
First ensure the dependencies are there:

    $ brew install cmake
    $ brew install ode --enable-double-precision
    $ brew install qt4
    $ brew install protobuf

If you run into build issues, you may need to run this first:

    $ brew update
    $ brew doctor

Next we need to install VarTypes manually :

> Note : the vartypes need some edits to be compatible with mac-osx so it's recommanded to use  :
> <https://github.com/lordhippo/vartypes.git> (branch -> osx-fix)


    $ (Get VarTypes)
    $ cd ./vartypes
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ sudo make install

Next, pull down the project:

    $ git clone https://github.com/mani-monaj/grSim.git
    $ cd ./grSim

Create a build directory within the project (this is ignored by .gitignore):

    $ mkdir build
    $ cd build

Run CMake to generate the makefiles:

    $ cmake ..

Then compile the program:

    $ make

The program is now under `../bin` directory with the name grSim. You can run it from here just by typing `../bin/grSim`;


you may want to use a cmake-gui instead of cmake, or maybe ccmake for ncurses fans.

The binary files (grSim and sample client) will be placed in `./bin`. Check the fps in status bar, if it is running on 65fps everything is ok, otherwise check the graphics card installation and OpenGL settings. If you received linker error about GL related libraries, also check your graphics card driver
