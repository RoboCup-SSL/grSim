cmake_minimum_required(VERSION 2.8)

set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(libs)

find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)
list(APPEND libs Qt5::Core Qt5::Widgets Qt5::Network)

find_package(protobuf CONFIG REQUIRED)
list(APPEND libs protobuf::libprotobuf)


set(app client)

add_executable(${app} MACOSX_BUNDLE
        main.cpp
        mainwindow.cpp
        mainwindow.h
        ../../src/proto/grSim_Replacement.proto
        ../../src/proto/grSim_Commands.proto
        ../../src/proto/grSim_Packet.proto
        )

target_link_libraries(${app} ${libs})

# Set the directory where the generated protobuf files will be placed
set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${PROTO_BINARY_DIR})
target_include_directories(${app} PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")

# Generate the sources from the .proto files
protobuf_generate(
        TARGET ${app}
        IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../src/proto"
        PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
)
