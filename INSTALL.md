We developed grSim on Ubuntu 9.10+ OS. (Ubuntu 11.04 tested and is recommended). It is very important that the Graphics Card Driver is installed. (Ubuntu packages of nVidia and AMD(ATI) graphics cards are available). grSim will compile and run in 32 bit Linux. 64 bit compile and run support is experimental.

To Install grSim on your system follow these steps:

1. Download and install following packages on your system:
   - libqt4-dev
   - libgl1-mesa-dev
   - libglu1-mesa-dev
   - VarTypes
   - ode
   - protobuf

2. Create build environment:
   ```
   mkdir build
   cd build
   cmake ..
   ```
   you may want to use a cmake-gui instead of cmake, or maybe ccmake for ncurses fans.

3. ``make``

The binary file will be in grSim/bin [Check the fps in status bar, if it is running on 65fps everything is ok, otherwise check the graphics card installation and OpenGL settings. If you received linker error about GL related libraries, also check your graphics card driver.

