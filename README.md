
[grSim](http://github.com/mani-monaj/grSim) [![Run Status](https://api.shippable.com/projects/5620224e1895ca44741e0b4d/badge?branch=2.0-dev)](https://app.shippable.com/projects/5620224e1895ca44741e0b4d)
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

grSim compiles on Linux (tested on Ubuntu variants only) as well as Mac OS and depends on the following libraries:

- Qt 4.8+ (provided by libqt4-dev on Debian based distributions)
- OpenGL
- Open Dynamics Engine (ODE)
- VarTypes Library
- Google ProtoBuf


Installation
------------

Please refer to INSTALL file.


Usage
-----

Receiving data from grSim is similar to receiving data from [SSL-Vision](https://github.com/RoboCup-SSL/ssl-vision) using [Google Protobuf](https://github.com/google/protobuf) library.
Sending data to Simulator is also possible using Google Protobuf. Sample clients are included in [clients](./clients) folder. There are two clients available, *qt-based* and *Java-based*. The native client is compiled during the grSim compilation. To compile the Java client, please consult the corresponding `README` file.


