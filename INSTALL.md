We developed grSim on Ubuntu 9.10+ OS. (Ubuntu 12.04+ tested and is recommended). It is very important that the Graphics Card Driver is installed. (Ubuntu packages of nVidia and AMD(ATI) graphics cards are available). grSim will compile and run in 32 bit Linux. 64 bit compile and run support is experimental.

grSim has dependcies on these external libraries in order to get compiled.

- Qt4 Development Libraries
- OpenGL
- Open Dynamics Engine (ODE)
- VarTypes Library 
- Google Protobuf

Except VarTypes, all other libraries can be installed using most Linux distributions' package managers.

In order to compile grSim on Ubuntu 12.04+, follow the following steps:

1. Download and install these packages:

```bash
sudo apt-get install build-essential cmake libqt4-dev libgl1-mesa-dev libglu1-mesa-dev libprotobuf-dev libode-dev
```

2. Download, Compild & Install VarTypes:

```bash
[In a temporary directory]
wget http://vartypes.googlecode.com/files/vartypes-0.7.tar.gz
tar xfz vartypes-0.7.tar.gz
cd vartypes-0.7
mkdir build && cd build
cmake ..
make 
sudo make install
```

3. Compile grSim

```
[In grSim folder]
mkdir build
cd build
cmake ..
make
```

you may want to use a cmake-gui instead of cmake, or maybe ccmake for ncurses fans.

The binary files (grSim and sample client)  will be placed in `grSim/bin`. Check the fps in status bar, if it is running on 65fps everything is ok, otherwise check the graphics card installation and OpenGL settings. If you received linker error about GL related libraries, also check your graphics card driver
