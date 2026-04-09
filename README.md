[![Build Status](https://github.com/RoboCup-SSL/grSim/workflows/Build/badge.svg)](https://github.com/RoboCup-SSL/grSim/actions?query=workflow%3ABuild+branch%3Amaster) [![CodeFactor](https://www.codefactor.io/repository/github/robocup-ssl/grsim/badge/master)](https://www.codefactor.io/repository/github/robocup-ssl/grsim/overview/master)

grSim
=======================

[RoboCup Small Size League](https://ssl.robocup.org/) Simulator.

![grSim on Ubuntu](docs/img/screenshot01.jpg?raw=true "grSim on Ubuntu")

- [Install instructions](INSTALL.md)
- [Authors](AUTHORS.md)
- [Changelog](CHANGELOG.md)
- License: [GNU General Public License (GPLv3)](LICENSE.md)

System Requirements
-----------------------

grSim will likely run on a modern dual-core PC with a decent graphics card. A typical configuration is:

- Dual Core CPU (2.0 Ghz+)
- 1GB of RAM
- 256MB nVidia or ATI graphics card

Note that it may run on lower-end equipment though good performance is not guaranteed.


Software Requirements
---------------------

grSim compiles on Linux (tested on Ubuntu and Arch Linux variants only) and Mac OS. It depends on the following libraries:

- [CMake](https://cmake.org/) version 3.5+
- [pkg-config](https://freedesktop.org/wiki/Software/pkg-config/)
- [OpenGL](https://www.opengl.org)
- [Qt5 Development Libraries](https://www.qt.io)
- [Open Dynamics Engine (ODE)](http://www.ode.org)
- [VarTypes Library](https://github.com/jpfeltracco/vartypes) forked from [Szi's Vartypes](https://github.com/szi/vartypes)
- [Google Protobuf](https://github.com/google/protobuf)
- [Boost development libraries](http://www.boost.org/) (needed by VarTypes)

Please consult the [install instructions](INSTALL.md) for more details.

Usage
-----

Receiving data from the grSim is similar to receiving data from the [SSL-Vision](https://github.com/RoboCup-SSL/ssl-vision) using [Google Protobuf](https://github.com/google/protobuf) library.
Sending data to the simulator is also possible using Google Protobuf. Sample clients are included in [clients](./clients) folder. There are two clients available, *qt-based* and *Java-based*. The native client is compiled during the grSim compilation. To compile the Java client, please consult the corresponding `README` file.

Qt [example project](https://github.com/robocin/ssl-client) to receive and send data to the simulator.

Star History
------
[![Star History Chart](https://api.star-history.com/svg?repos=robocup-ssl/grsim&type=Date)](https://star-history.com/#robocup-ssl/grsim&Date)

Citing
------

If you use this in your research, please cite the original paper:
```
@inproceedings{Monajjemi2011grSimR,
  title={grSim - RoboCup Small Size Robot Soccer Simulator},
  author={Valiallah Monajjemi and A. Koochakzadeh and S. S. Ghidary},
  booktitle={RoboCup},
  year={2011}
}
```

If you wish to cite this repo with it's modifications specifically, please cite:

```
@misc{grsim2021,
  author = {Mohammad Mahdi Rahimi and Jan Segre and Valiallah Monajjemi and A. Koochakzadeh and Sepehr MohaimenianPour and Nicolai Ommer and  Avatar
Kazunori Kimura and Jeremy Feltracco and Kenta Sato and Atousa Ahsani},
  title = {GRSIM},
  year = {2021},
  publisher = {GitHub},
  note = {GitHub repository},
  howpublished = {\url{https://github.com/RoboCup-SSL/grSim/}}
}
```
