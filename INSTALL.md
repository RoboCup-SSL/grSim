# GrSim - INSTALL

## Overview

We developed grSim on Ubuntu OS. (Ubuntu 14.04+ tested and is recommended). It is  important that the graphics card driver is installed properly (the official Ubuntu packages for nVidia and AMD(ATI) graphics cards are available). grSim will compile and run in both 32 and 64 bits Linux and Mac OS.

GrSim is written in C++, in order to compile it, you will need a working toolchain and a c++ compiler.

## Dependencies

GrSim depends on:

- [CMake](https://cmake.org/) version 2.8+ 
- [OpenGL](https://www.opengl.org)
- [Qt4 Development Libraries](https://www.qt.io) version 4.8+
- [Open Dynamics Engine (ODE)](http://www.ode.org)
- [VarTypes Library](https://github.com/szi/vartypes)
- [Google Protobuf](https://github.com/google/protobuf)
- [Boost development libraries](http://www.boost.org/) (needed by VarTypes)

**Note:** It's necessary to compile ODE in double precision. This is default when installing the ODE binaries in Ubuntu. However, if you are compiling ODE from source (e.g on Mac OS), please make sure to enable the double precision during the configuration step: `./configure --enable-double-precision`.

### Linux/Unix Installation

If you run a Debian system, or derivative, first ensure that these dependencies are there:

```bash
$ sudo apt-get install git build-essential cmake libqt4-dev libgl1-mesa-dev libglu1-mesa-dev libprotobuf-dev protobuf-compiler libode-dev libboost-dev
```

Next compile and install VarTypes from source. In the following we install VarTypes from source using `git`.

```bash
$ cd /tmp
$ git clone https://github.com/szi/vartypes.git 
$ cd vartypes
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```

Next, clone grSim into your preferred location.

```bash
$ cd /path/to/grsim_ws
$ git clone https://github.com/mani-monaj/grSim.git
$ cd grSim
```

Create a build directory within the project (this is ignored by .gitignore):

```bash
$ mkdir build
$ cd build
```

Run CMake to generate the makefiles:

```bash
$ cmake ..
```

Then compile the program:

```bash
$ make
```

The binary is copied to the `../bin` folder after a successful compilation.

### Mac OS X Installation

Pre-requirements:

- [Xcode](https://developer.apple.com/xcode/) or Xcode Command Line Tools 8.0 or newer
- [Homebrew](http://brew.sh/) package manager.

First ensure the dependencies are there:

```bash
$ brew install cmake
$ brew install ode --with-double-precision
$ brew tap cartr/qt4
$ brew tap-pin cartr/qt4
$ brew install qt@4  
$ brew install protobuf
```

If you run into build issues, you may need to run this first:

```bash
$ brew update
$ brew doctor
```

Next we need to install VarTypes manually. Please refer to the documentation above for the procedure. 

**IMPORTANT NOTE:** VarTypes currently does not compile on MacOS. Please use the following fork of Vartypes instead of the official repository: `https://github.com/lordhippo/vartypes.git (branch: osx-fix)`

The steps to compile grSim on Mac OS is similar to the steps outlines above for Linux:


```bash
$ cd /path/to/grsim_ws
$ git clone https://github.com/mani-monaj/grSim.git
$ cd ./grSim
$ mkdir build
$ cd build
$ cmake ..
$ make
```

The binary files (grSim and the sample client) will be placed in `../bin`. 

## Notes on the performance

When running grSim, check the FPS in the status bar. If it is running at **60 FPS** or higher, everything is ok. Otherwise check the graphics card's driver installation and OpenGL settings.
