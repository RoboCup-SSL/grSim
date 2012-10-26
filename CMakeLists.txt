cmake_minimum_required(VERSION 2.8)

project(grSim)

set(VERSION "1.0.0a2")
set(MAINTAINER "Jan Segre <jan@segre.in>")
set(VENDOR "RoboIME")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
include(${PROJECT_SOURCE_DIR}/cmake/Utils.cmake)

standard_config()
standard_paths(${PROJECT_SOURCE_DIR} bin lib)

if(MSVC)
    add_definitions(-DHAVE_MSVC)
endif()

if(WIN32)
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
    src/graphics.cpp
    src/physics/pworld.cpp
    src/physics/pobject.cpp
    src/physics/pball.cpp
    src/physics/pground.cpp
    src/physics/pfixedbox.cpp
    src/physics/pcylinder.cpp
    src/physics/pbox.cpp
    src/physics/pray.cpp
    src/net/robocup_ssl_server.cpp
    src/net/robocup_ssl_client.cpp
    src/sslworld.cpp
    src/robot.cpp
    src/configwidget.cpp
    src/statuswidget.cpp
    src/logger.cpp
    src/robotwidget.cpp
    src/getpositionwidget.cpp
    resources/grsim.rc
)

set(HEADERS
    src/mainwindow.h
    src/glwidget.h
    src/graphics.h
    src/physics/pworld.h
    src/physics/pobject.h
    src/physics/pball.h
    src/physics/pground.h
    src/physics/pfixedbox.h
    src/physics/pcylinder.h
    src/physics/pbox.h
    src/physics/pray.h
    src/net/robocup_ssl_server.h
    src/net/robocup_ssl_client.h
    src/sslworld.h
    src/robot.h
    src/configwidget.h
    src/statuswidget.h
    src/logger.h
    src/robotwidget.h
    src/getpositionwidget.h
    src/common.h
    src/config.h
)

file(GLOB CONFIG_FILES "config/*.ini")
set_source_files_properties(${CONFIG_FILES}  PROPERTIES MACOSX_PACKAGE_LOCATION "config")

set(srcs
    ${CONFIG_FILES}
    ${PROTO_CPP}
    ${PROTO_H}
    ${RESOURCES}
    ${HEADERS}
    ${SOURCES}
)

if(APPLE)
  set(app grSim)
  add_executable(${app} MACOSX_BUNDLE ${srcs})
  target_link_libraries(${app} ${libs})
  install(TARGETS ${app} DESTINATION .)
  set(MACOSX_BUNDLE_ICON_FILE "${PROJECT_SOURCE_DIR}/resources/icons/grsim.icns")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION})
  set(MACOSX_BUNDLE_VERSION ${VERSION})
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${VERSION}")
  #set(CMAKE_OSX_ARCHITECTURES x86_64 i386 ppc)
  set(BUNDLE_APP ${PROJECT_SOURCE_DIR}/bin/${app}.app)
  install(
      CODE "
      include(BundleUtilities)
      fixup_bundle(\"${BUNDLE_APP}\"   \"\"   \"/opt/local/lib;/usr/local/lib\")"
      COMPONENT Runtime)
  set(CPACK_GENERATOR "DragNDrop" "TGZ")
elseif(WIN32)
  set(app grSim)
  add_executable(${app} WIN32 ${srcs})
  target_link_libraries(${app} ${libs})
  install(TARGETS ${app} DESTINATION bin)
  install(DIRECTORY config DESTINATION .)
  install(DIRECTORY bin DESTINATION .
          FILES_MATCHING PATTERN "*.dll")
  set(CPACK_PACKAGE_EXECUTABLES ${app} grSim)
else()
  set(app grsim)
  add_executable(${app} ${srcs})
  target_link_libraries(${app} ${libs})
  install(TARGETS ${app} DESTINATION bin)
  install(DIRECTORY config DESTINATION share/${app})
  install(FILES resources/grsim.desktop DESTINATION share/applications)
  install(FILES resources/icons/grsim.svg DESTINATION share/icons/hicolor/scalable/apps)
endif()

option(BUILD_CLIENTS "Choose this option if you want to build the example Qt client." ON)
if(BUILD_CLIENTS)
    add_subdirectory(clients/qt)
endif()

file(COPY README.md LICENSE.md DESTINATION ${CMAKE_BINARY_DIR})
file(RENAME ${CMAKE_BINARY_DIR}/README.md ${CMAKE_BINARY_DIR}/README.txt)
file(RENAME ${CMAKE_BINARY_DIR}/LICENSE.md ${CMAKE_BINARY_DIR}/LICENSE.txt)

## Packaging
if(UNIX)
  execute_process(COMMAND uname -p OUTPUT_VARIABLE ARCH)
  string(STRIP ${ARCH} ARCH)
  if(APPLE)
    set(ARCH "osx-universal")
  endif()
elseif(WIN32)
  set(ARCH "win32")
  set(CPACK_GENERATOR ZIP NSIS)
endif()
set(CPACK_OUTPUT_FILE_PREFIX ${PROJECT_SOURCE_DIR}/dist)
set(CPACK_PACKAGE_CONTACT ${MAINTAINER})
set(CPACK_PACKAGE_NAME "grsim")
set(CPACK_PACKAGE_VENDOR ${VENDOR})
string(TOLOWER ${CPACK_PACKAGE_VENDOR} FLAVOR)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_BINARY_DIR}/README.txt")
#set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/WELCOME.txt")
# TODO: is this easy enough to maintain?
# There doesn't seems to be any version so far.
# As this fork has possibly incompatible modifications, and assuming the original
# version was 0.X.X then by the semver principles this should be 1.0.0 when released
set(CPACK_PACKAGE_VERSION ${VERSION})
# Debian based specific
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libode1 (>=0.11), vartypes (>=0.7.0), libqt4-network (>=4.8.0), libqt4-core, libqt4-gui, libqt4-opengl")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}-${FLAVOR}_${ARCH}")
include(CPack)

