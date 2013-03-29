grSim
=====

RoboCup Small Size League Simulator


System Requirements
-----------------------

grSim will likely run on a modern dual core PC with good graphics card. Typical configuration is:

- Dual Core CPU (2.0 Ghz+)
- 1GB of RAM
- 256MB nVidia or ATI graphics card

Note that it may run on lower end equipment though good performance is not guaranteed.


Software Requirements
---------------------

grSim uses these libraries:

- Qt 4.8+ (provided by libqt4-dev on debian based distros)
- OpenGL
- Open Dynamics Engine (ODE)
- VarTypes Library
- Google ProtoBuf


Installation
------------

Please refer to INSTALL file.


Usage
-----

Receiving data from grSim is exactly like receiving from [SSL-Vision](http://code.google.com/p/ssl-vision) using Google Protobuf library
Sending data to Simulator is also possible using Google Protobuf. Sample clients are included in [clients](./clients) folder. There are two clients available, qt-based and Java-based. The native client is compiled during the grSim compilation. To compile the Java client, please consult client's `README` file.

Changelog
---------

Please refer to the CHANGELOG file.

