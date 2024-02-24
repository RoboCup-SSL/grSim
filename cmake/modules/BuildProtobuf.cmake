# ***************************************************************************
# *   Copyright 2017 Michael Eischer                                        *
# *   Robotics Erlangen e.V.                                                *
# *   http://www.robotics-erlangen.de/                                      *
# *   info@robotics-erlangen.de                                             *
# *                                                                         *
# *   This program is free software: you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation, either version 3 of the License, or     *
# *   any later version.                                                    *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
# ***************************************************************************

include(ExternalProject)

set(PROTOBUF_CMAKE_ARGS )

ExternalProject_Add(protobuf_external
                    # URL is the same as in the ER-Force framework,
                    # because ER-Force needs it and has an incentive to keep the link stable
  URL               http://www.robotics-erlangen.de/downloads/libraries/protobuf-cpp-3.6.1.tar.gz
  URL_HASH          SHA256=b3732e471a9bb7950f090fd0457ebd2536a9ba0891b7f3785919c654fe2a2529
  SOURCE_SUBDIR     cmake
  CMAKE_ARGS
                    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
                    -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
                    -DCMAKE_C_COMPILER:PATH=${CMAKE_C_COMPILER}
                    -DCMAKE_CXX_COMPILER:PATH=${CMAKE_CXX_COMPILER}
                    -DCMAKE_MAKE_PROGRAM:PATH=${CMAKE_MAKE_PROGRAM}
                    # the tests fail to build :-(
                    -Dprotobuf_BUILD_TESTS:BOOL=OFF
  STEP_TARGETS install
)

set(PROTOBUF_SUBPATH "${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}protobuf${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(LIBPROTOC_SUBPATH "${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}protoc${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(PROTOC_SUBPATH "bin/protoc${CMAKE_EXECUTABLE_SUFFIX}")

# the byproducts are available after the install step
ExternalProject_Add_Step(protobuf_external out
    DEPENDEES install
    BYPRODUCTS
        "<INSTALL_DIR>/${PROTOBUF_SUBPATH}"
        "<INSTALL_DIR>/${LIBPROTOC_SUBPATH}"
        "<INSTALL_DIR>/${PROTOC_SUBPATH}"
)

ExternalProject_Get_Property(protobuf_external install_dir)
set_target_properties(protobuf_external PROPERTIES EXCLUDE_FROM_ALL true)

# override all necessary variables originally set by find_package
# if FORCE is not set cmake does not allow us to override the variables, for some unknown reason
set(Protobuf_FOUND true CACHE BOOL "" FORCE)
set(Protobuf_VERSION "3.6.1" CACHE STRING "" FORCE)
set(Protobuf_INCLUDE_DIR "${install_dir}/include" CACHE PATH "" FORCE)
set(Protobuf_INCLUDE_DIRS "${Protobuf_INCLUDE_DIR}" CACHE PATH "" FORCE)
set(Protobuf_LIBRARY "${install_dir}/${PROTOBUF_SUBPATH}" CACHE PATH "" FORCE)
set(Protobuf_LIBRARIES "${Protobuf_LIBRARY}" CACHE PATH "" FORCE)
set(Protobuf_LIBRARY_DEBUG "${install_dir}/${PROTOBUF_SUBPATH}" CACHE PATH "" FORCE)
set(Protobuf_LIBRARY_RELEASE "${install_dir}/${PROTOBUF_SUBPATH}" CACHE PATH "" FORCE)
set(Protobuf_LITE_LIBRARY_DEBUG "${install_dir}/${PROTOBUF_SUBPATH}" CACHE PATH "" FORCE)
set(Protobuf_LITE_LIBRARY_RELEASE "${install_dir}/${PROTOBUF_SUBPATH}" CACHE PATH "" FORCE)
set(Protobuf_PROTOC_EXECUTABLE "${install_dir}/${PROTOC_SUBPATH}" CACHE PATH "" FORCE)
set(Protobuf_PROTOC_LIBRARY_DEBUG "${install_dir}/${LIBPROTOC_SUBPATH}" CACHE PATH "" FORCE)
set(Protobuf_PROTOC_LIBRARY_RELEASE "${install_dir}/${LIBPROTOC_SUBPATH}" CACHE PATH "" FORCE)
# this is a dependency for the protobuf_generate_cpp custom command
# if this is not set the generate command sometimes get executed before protoc is compiled
set(protobuf_generate_DEPENDENCIES protobuf_external CACHE STRING "" FORCE)

# compatibility with cmake 3.10
if(NOT TARGET protobuf::protoc)
    # avoid error if target was already created for an older version
    add_executable(protobuf::protoc IMPORTED)
endif()
# override the protobuf::protoc path used by the protobuf_generate_cpp macro
set_target_properties(protobuf::protoc PROPERTIES
    IMPORTED_LOCATION "${Protobuf_PROTOC_EXECUTABLE}"
)

message(STATUS "Building protobuf ${Protobuf_VERSION}")
