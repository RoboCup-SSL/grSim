
grSim-[![Build Status](https://travis-ci.org/RoboCup-SSL/grSim.svg?branch=master)](https://travis-ci.org/RoboCup-SSL/grSim)
=======================

[RoboCup Small Size League](http://wiki.robocup.org/Small_Size_League) Simulator.

![grSim on Ubuntu](docs/img/screenshot01.jpg?raw=true "grSim on Ubuntu")

- [Install instructions](INSTALL.md)
- [Authors](AUTHORS.md)
- [Changelog](CHANGELOG.md)
- License: [GNU General Public License (GPLv3)](LICENSE.md)

System Requirements
-----------------------

grSim will likely run on a modern dual core PC with a decent graphics card. Typical configuration is:

- Dual Core CPU (2.0 Ghz+)
- 1GB of RAM
- 256MB nVidia or ATI graphics card

Note that it may run on lower end equipment though good performance is not guaranteed.


Software Requirements
---------------------

grSim compiles on Linux (tested on Ubuntu variants only) and Mac OS. It depends on the following libraries:

- [CMake](https://cmake.org/) version 2.8+ 
- [OpenGL](https://www.opengl.org)
- [Qt4 Development Libraries](https://www.qt.io) version 4.8+
- [Open Dynamics Engine (ODE)](http://www.ode.org)
- [VarTypes Library](https://github.com/szi/vartypes)
- [Google Protobuf](https://github.com/google/protobuf)
- [Boost development libraries](http://www.boost.org/) (needed by VarTypes)

Please consult the [install instructions](INSTALL.md) for more details.

Usage
-----

Receiving data from grSim is similar to receiving data from [SSL-Vision](https://github.com/RoboCup-SSL/ssl-vision) using [Google Protobuf](https://github.com/google/protobuf) library.
Sending data to the simulator is also possible using Google Protobuf. Sample clients are included in [clients](./clients) folder. There are two clients available, *qt-based* and *Java-based*. The native client is compiled during the grSim compilation. To compile the Java client, please consult the corresponding `README` file.

Qt [example project](https://github.com/robocin/ssl-client) to receive and send data to the simulator.


Citing
------

If you use grSim in your research, please cite [the following paper](http://link.springer.com/chapter/10.1007/978-3-642-32060-6_38):

> Monajjemi, Valiallah (Mani), Ali Koochakzadeh, and Saeed Shiry Ghidary. "grSim – RoboCup Small Size Robot Soccer Simulator." In Robot Soccer World Cup, pp. 450-460. Springer Berlin Heidelberg, 2011.
