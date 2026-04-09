FROM ubuntu:20.04 AS build
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    git \
    build-essential \
    cmake \
    pkg-config \
    qt5-default \
    libqt5opengl5-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libprotobuf-dev \
    protobuf-compiler \
    libode-dev \
    libboost-dev

WORKDIR /vartypes
RUN git clone https://github.com/jpfeltracco/vartypes.git . && \
    git checkout 2d16e81b7995f25c5ba5e4bc31bf9a514ee4bc42 && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

WORKDIR /grsim
COPY clients /grsim/clients
COPY cmake /grsim/cmake
COPY config /grsim/config
COPY include /grsim/include
COPY resources /grsim/resources
COPY src /grsim/src
COPY CMakeLists.txt README.md LICENSE.md /grsim/
RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && \
    make && \
    make install


FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive \
    LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib

RUN apt-get update && apt-get install -y \
        tini \
        qt5-default \
        libqt5opengl5 \
        libode8 \
        libprotobuf17 \
        # virtual display and VNC server
        x11vnc xvfb && \
        apt-get clean -y
COPY --from=build /usr/local /usr/local

RUN useradd -ms /bin/bash default
COPY /docker-entry.sh .
RUN chmod 775 /docker-entry.sh

EXPOSE 20011 30011 30012 10300 10301 10302 5900
USER default
WORKDIR /home/default
ENTRYPOINT ["tini", "--", "/docker-entry.sh"]
