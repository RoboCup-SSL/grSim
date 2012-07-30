cmake_minimum_required(VERSION 2.8)

project(grSim)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
include(${PROJECT_SOURCE_DIR}/cmake/Utils.cmake)

standard_config()
standard_paths(${PROJECT_SOURCE_DIR} bin lib)

if(MSVC)
    add_definitions(-DHAVE_WINDOWS)
endif()

if(UNIX)
    add_definitions(-DHAVE_UNIX)
    if(APPLE)
        add_definitions(-DHAVE_MACOSX)
    else()
        #TODO: fix this, say we have FreeBSD, that's not linux
        add_definitions(-DHAVE_LINUX)
    endif()
endif()

include_directories(src)

# placeholder of used libs
set(libs)

# OpenGL stuff
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
list(APPEND libs ${OPENGL_LIBRARIES})
#find_package(GLUT REQUIRED)
#include_directories(${GLUT_INCLUDE_DIR})

# Qt stuff
set(QT_USE_QTNETWORK true)
set(QT_USE_QTOPENGL true)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
list(APPEND libs ${QT_LIBRARIES})

# ODE stuff
find_package(ODE REQUIRED)
include_directories(${ODE_INCLUDE_DIRS})
list(APPEND libs ${ODE_LIBRARIES})
#TODO: make this an option
option(DOUBLE_PRECISION "Use double precision? If not single precision will be used." ON)
if(DOUBLE_PRECISION)
    add_definitions(-DdDOUBLE)
else()
    add_definitions(-DdSINGLE)
endif()


# VarTypes stuff
find_package(VarTypes REQUIRED)
include_directories(${VARTYPES_INCLUDE_DIRS})
list(APPEND libs ${VARTYPES_LIBRARIES})

# Protobuf stuff
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
list(APPEND libs ${PROTOBUF_LIBRARIES})

protobuf_generate_cpp(PROTO_CPP PROTO_H
    src/proto/messages_robocup_ssl_detection.proto
    src/proto/messages_robocup_ssl_geometry.proto
    src/proto/messages_robocup_ssl_wrapper.proto
    src/proto/messages_robocup_ssl_refbox_log.proto
    src/proto/grSim_Replacement.proto
    src/proto/grSim_Commands.proto
    src/proto/grSim_Packet.proto
)

qt4_add_resources(RESOURCES
    src/textures.qrc
)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/glwidget.cpp
    src/Graphics/graphics.cpp
    src/Physics/pworld.cpp
    src/Physics/pobject.cpp
    src/Physics/pball.cpp
    src/Physics/pground.cpp
    src/sslworld.cpp
    src/Physics/pfixedbox.cpp
    src/Physics/pcylinder.cpp
    src/robot.cpp
    src/Physics/pbox.cpp
    src/net/robocup_ssl_server.cpp
    src/net/robocup_ssl_client.cpp
    src/Physics/pray.cpp
    src/configwidget.cpp
    src/statuswidget.cpp
    src/logger.cpp
    src/robotwidget.cpp
    src/getpositionwidget.cpp
)

set(HEADERS
    src/mainwindow.h
    src/glwidget.h
    src/Graphics/graphics.h
    src/Physics/pworld.h
    src/Physics/pobject.h
    src/Physics/pball.h
    src/Physics/pground.h
    src/sslworld.h
    src/Physics/pfixedbox.h
    src/Physics/pcylinder.h
    src/robot.h
    src/Physics/pbox.h
    src/net/robocup_ssl_server.h
    src/net/robocup_ssl_client.h
    src/Physics/pray.h
    src/configwidget.h
    src/statuswidget.h
    src/logger.h
    src/robotwidget.h
    src/getpositionwidget.h
    src/common.h
)

file(GLOB CONFIG_FILES "config/*.ini")
set_source_files_properties(${CONFIG_FILES}  PROPERTIES MACOSX_PACKAGE_LOCATION "config")

set(app grSim)

add_executable(${app} MACOSX_BUNDLE
    ${CONFIG_FILES}
    ${PROTO_CPP}
    ${PROTO_H}
    ${RESOURCES}
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(${app} ${libs})

option(BUILD_CLIENTS "Choose this option if you want to build the example Qt client." ON)
if(BUILD_CLIENTS)
    add_subdirectory(clients/qt)
endif()

