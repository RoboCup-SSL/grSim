cmake_minimum_required(VERSION 3.5)


## Project branding, version and package mantainer
project(grSim)
set(VERSION "1.0.0a2")
#set(VENDOR "Parsian")
set(MAINTAINER "Mani Monajjemi <mani.monajjemi@gmail.com>")


# some utils and helper vars
string(TOLOWER ${CMAKE_PROJECT_NAME} CMAKE_PROJECT_NAME_LOWER)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
include(${PROJECT_SOURCE_DIR}/cmake/Utils.cmake)
standard_config()
standard_paths(${PROJECT_SOURCE_DIR} bin lib)

set(app ${CMAKE_PROJECT_NAME})
# create the target before the sources list is known so that we can call
# add_dependencies(<target> external_proj)
add_executable(${app} "")

# definitions for knowing the OS from the code
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

# set explicitly the c++ standard to use
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# add src dir to included directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/net)
include_directories(${PROJECT_SOURCE_DIR}/include/physics)


## Handling depenendcies

# we will append all libs to this var
set(libs)

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
list(APPEND libs ${OPENGL_LIBRARIES})
#find_package(GLUT REQUIRED)
#include_directories(${GLUT_INCLUDE_DIR})

# Qt
if(APPLE AND EXISTS /usr/local/opt/qt)
  # Homebrew installs Qt5 (up to at least 5.9.1) in
  # /usr/local/qt5, ensure it can be found by CMake since
  # it is not in the default /usr/local prefix.
  list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt")
endif()
find_package(Qt5 COMPONENTS Core Widgets OpenGL Network REQUIRED)
list(APPEND libs Qt5::Core Qt5::Widgets Qt5::OpenGL Qt5::Network)

# ODE
if(WIN32)
  find_package(ODE CONFIG REQUIRED)
  list(APPEND libs ODE::ODE)
else()
  find_package(ODE REQUIRED)
  list(APPEND libs ode::ode)
endif()

# VarTypes
find_package(VarTypes)

if(NOT VARTYPES_FOUND)
  include(ExternalProject)
  set(VARTYPES_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/vartypes_install")
  
  set(VARTYPES_CMAKE_ARGS "-DVARTYPES_BUILD_STATIC=ON;-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>")
  if(NOT "${CMAKE_TOOLCHAIN_FILE}" STREQUAL "")
    set(VARTYPES_CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE};${VARTYPES_CMAKE_ARGS}")
  endif()

  ExternalProject_Add(vartypes_external
    GIT_REPOSITORY    https://github.com/jpfeltracco/vartypes
    GIT_TAG           origin/jpfeltracco/build_static
    INSTALL_DIR       "${VARTYPES_INSTALL_DIR}"
    CMAKE_ARGS        "${VARTYPES_CMAKE_ARGS}"
  )
  add_dependencies(${app} vartypes_external)

  set(VARTYPES_INCLUDE_DIRS "${VARTYPES_INSTALL_DIR}/include")
  set(VARTYPE_LIB_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}vartypes${CMAKE_STATIC_LIBRARY_SUFFIX})
  set(VARTYPES_LIBRARIES "${VARTYPES_INSTALL_DIR}/lib/${VARTYPE_LIB_NAME}")
endif()

target_include_directories(${app} PRIVATE ${VARTYPES_INCLUDE_DIRS})
list(APPEND libs ${VARTYPES_LIBRARIES})

# Protobuf
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
list(APPEND libs ${PROTOBUF_LIBRARIES})

set (Protobuf_IMPORT_DIRS ${Protobuf_INCLUDE_DIR})
protobuf_generate_cpp(PROTO_CPP PROTO_H
    src/proto/grSim_Commands.proto
    src/proto/grSim_Packet.proto
    src/proto/grSim_Replacement.proto
    src/proto/grSim_Robotstatus.proto
    src/proto/ssl_gc_common.proto
    src/proto/ssl_simulation_config.proto
    src/proto/ssl_simulation_control.proto
    src/proto/ssl_simulation_error.proto
    src/proto/ssl_simulation_robot_control.proto
    src/proto/ssl_simulation_robot_feedback.proto
    src/proto/ssl_simulation_synchronous.proto
    src/proto/ssl_vision_detection.proto
    src/proto/ssl_vision_geometry.proto
    src/proto/ssl_vision_wrapper.proto
)

qt5_add_resources(RESOURCES
    resources/textures.qrc
)

set(RESOURCES
    ${RESOURCES}
    resources/grsim.rc
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
)

set(HEADERS
    include/mainwindow.h
    include/glwidget.h
    include/graphics.h
    include/physics/pworld.h
    include/physics/pobject.h
    include/physics/pball.h
    include/physics/pground.h
    include/physics/pfixedbox.h
    include/physics/pcylinder.h
    include/physics/pbox.h
    include/physics/pray.h
    include/net/robocup_ssl_server.h
    include/net/robocup_ssl_client.h
    include/sslworld.h
    include/robot.h
    include/configwidget.h
    include/statuswidget.h
    include/logger.h
    include/robotwidget.h
    include/getpositionwidget.h
    include/common.h
    include/config.h
)

# files to be compiled
set(srcs
    ${CONFIG_FILES}
    ${PROTO_CPP}
    ${PROTO_H}
    ${RESOURCES}
    ${HEADERS}
    ${SOURCES}
)

file(GLOB CONFIG_FILES "config/*.ini")
set_source_files_properties(${CONFIG_FILES}  PROPERTIES MACOSX_PACKAGE_LOCATION "config")

target_sources(${app} PRIVATE ${srcs})
install(TARGETS ${app} DESTINATION bin)
target_link_libraries(${app} ${libs})

if(APPLE AND CMAKE_MACOSX_BUNDLE)
  # use CMAKE_MACOSX_BUNDLE if you want to build a mac bundle
  set(MACOSX_BUNDLE_ICON_FILE "${PROJECT_SOURCE_DIR}/resources/icons/grsim.icns")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION})
  set(MACOSX_BUNDLE_VERSION ${VERSION})
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${VERSION}")
  set(BUNDLE_APP ${PROJECT_SOURCE_DIR}/bin/${app}.app)
  install(
      CODE "
      include(BundleUtilities)
      fixup_bundle(\"${BUNDLE_APP}\"   \"\"   \"/opt/local/lib;/usr/local/lib\")"
      COMPONENT Runtime)
  set(CPACK_GENERATOR "DragNDrop" "TGZ")
elseif(WIN32 AND CMAKE_WIN32_EXECUTABLE)
  # use CMAKE_WIN32_EXECUTABLE if you want to build a windows exe
  install(DIRECTORY config DESTINATION .)
  install(DIRECTORY bin DESTINATION .
          FILES_MATCHING PATTERN "*.dll")
  set(CPACK_PACKAGE_EXECUTABLES ${app} ${app})
else()
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
if(VENDOR)
    set(CPACK_PACKAGE_VENDOR ${VENDOR})
    string(TOLOWER ${CPACK_PACKAGE_VENDOR} FLAVOR)
endif()
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_BINARY_DIR}/README.txt")
#set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/WELCOME.txt")
set(CPACK_PACKAGE_VERSION ${VERSION})
# Debian based specific
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libode1 (>=0.11), vartypes (>=0.7.0)")
if(FLAVOR)
    set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME_LOWER}_${CPACK_PACKAGE_VERSION}-${FLAVOR}_${ARCH}")
else()
    set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME_LOWER}_${CPACK_PACKAGE_VERSION}_${ARCH}")
endif()
include(CPack)


