cmake_minimum_required(VERSION 2.8)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(libs)

set(QT_USE_QTNETWORK TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
list(APPEND libs ${QT_LIBRARIES})

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
list(APPEND libs ${PROTOBUF_LIBRARIES})

protobuf_generate_cpp(PROTO_CPP PROTO_H
    ../../src/proto/grSim_Replacement.proto
    ../../src/proto/grSim_Commands.proto
    ../../src/proto/grSim_Packet.proto
)

set(app client)

add_executable(${app} MACOSX_BUNDLE
    ${PROTO_CPP}
    ${PROTO_H}
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

target_link_libraries(${app} ${libs})

