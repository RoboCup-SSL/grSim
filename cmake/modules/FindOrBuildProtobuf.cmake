# sanitize environment before find_package, because otherwise it also looks in the directory created for the ExternalProject
include(EnvHelper)
sanitize_env()
find_package(Protobuf REQUIRED)
restore_env()

# protobuf versions >= 3.21 are incompatible with how the project is setup and cause weird errors
# so we build protobuf ourselves
if(Protobuf_VERSION VERSION_GREATER_EQUAL 3.21)
  include(BuildProtobuf)
endif()
