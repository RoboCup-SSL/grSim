# GrSim - INSTALL

## Overview

We developed grSim on Ubuntu OS. (Ubuntu 14.04+ tested and is recommended). It is  important that the graphics card driver is installed properly (the official Ubuntu packages for nVidia and AMD(ATI) graphics cards are available). grSim will compile and run in both 32 and 64 bits Linux and Mac OS.

GrSim is written in C++, in order to compile it, you will need a working toolchain and a c++ compiler.

## Dependencies

GrSim depends on:

- [CMake](https://cmake.org/) version 3.5+
- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)
- [OpenGL](https://www.opengl.org)
- [Qt5 Development Libraries](https://www.qt.io)
- [Open Dynamics Engine (ODE)](http://www.ode.org)
- [VarTypes Library](https://github.com/jpfeltracco/vartypes) forked from [Szi's Vartypes](https://github.com/szi/vartypes)
- [Google Protobuf](https://github.com/google/protobuf)
- [Boost development libraries](http://www.boost.org/) (needed by VarTypes)

**Note:** It's necessary to compile ODE in double precision. This is default when installing the ODE binaries in Ubuntu. However, if you are compiling ODE from source (e.g on Mac OS), please make sure to enable the double precision during the configuration step: `./configure --enable-double-precision`.



## Installing from package manager
### Arch Linux

A package of grSim is avaliable on the [Arch User Repository](https://aur.archlinux.org/packages/grsim-git/), you can install it with your preferred AUR manager. Using `yay` it can be done with:
```bash
$ sudo yay -S grsim-git
```

## Building and installing from the source code

### Installing Dependencies

#### Arch Linux

If you are running Arch Linux or an Arch Linux based distribution, install the dependencies with:
```
$ sudo pacman -S base-devel boost hicolor-icon-theme \
                 mesa ode protobuf qt5-base cmake git
```

#### Debian

For Debian, or derivative
```
$ sudo apt install git build-essential cmake pkg-config qt5-default \
                   libqt5opengl5-dev libgl1-mesa-dev libglu1-mesa-dev \
                   libprotobuf-dev protobuf-compiler libode-dev libboost-dev
```

#### Mac OS X

For Mac OS X, you will need to have installed:

- [Xcode](https://developer.apple.com/xcode/) or Xcode Command Line Tools 8.0 or newer;
- [Homebrew](http://brew.sh/) package manager.

Than install the dependencies needed:

```bash
brew install cmake
brew install pkg-config
brew tap robotology/formulae         
brew install robotology/formulae/ode
brew install qt
brew install protobuf
```

If you run into build issues, you may need to run this first:

```bash
brew update
brew doctor
```

### Building

First clone grSim into your preferred location.

```bash
$ cd /path/to/grsim_ws
$ git clone https://github.com/RoboCup-SSL/grSim.git
$ cd grSim
```

Create a build directory within the project (this is ignored by .gitignore):

```bash
$ mkdir build
$ cd build
```

Run CMake to generate the makefile (note: if you proceed with the installation, grSim will be installed into directory chosen, by default `/usr/local`):

```bash
$ cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
```

Then compile the program:

```bash
$ make
```

The executable will be located on the `bin` directory.

### Installing

At least, if you want to install grSim on your system, run the follow:

```bash
$ sudo make install
```

grSim will be — by default — installed on the `/usr/local` directory.


## Notes on the performance

When running grSim, check the FPS in the status bar. If it is running at **60 FPS** or higher, everything is ok. Otherwise check the graphics card's driver installation and OpenGL settings.
