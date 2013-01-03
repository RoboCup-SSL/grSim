find_path(
   VARTYPES_INCLUDE_DIRS
   NAMES
   vartypes/VarTypes.h
   HINTS
   $ENV{HOME}/include
   /usr/local/include
   /usr/include
   $ENV{ProgramFiles}/vartypes/include
)

find_library(
   VARTYPES_LIBRARY
   NAMES
   vartypes
   HINTS
   $ENV{HOME}/lib
   /usr/local/lib
   /usr/lib
   $ENV{ProgramFiles}/vartypes/lib
)

set(VARTYPES_LIBRARIES ${VARTYPES_LIBRARY})

find_package_handle_standard_args(
   VARTYPES
   DEFAULT_MSG
   VARTYPES_INCLUDE_DIRS
   VARTYPES_LIBRARIES
)

mark_as_advanced(
    VARTYPES_INCLUDE_DIRS
    VARTYPES_LIBRARIES
    VARTYPES_LIBRARY
)

