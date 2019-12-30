find_package(PkgConfig QUIET)

foreach(pc_lib ode-double ode)
  pkg_check_modules(pc_ode QUIET "${pc_lib}")
  if(pc_ode_FOUND)
    if(pc_ode_VERSION VERSION_LESS "0.13")
      if (pc_ode_CFLAGS MATCHES "-D(dSINGLE|dDOUBLE)")
        set(pc_ode_precision "${CMAKE_MATCH_1}")
      endif()
    else()
      pkg_get_variable(pc_ode_precision "${pc_lib}" precision)
    endif()

    break()
  endif()
endforeach()

find_library(ODE_LIBRARIES
  NAMES "${pc_ode_LIBRARIES}"
  HINTS "${pc_ode_LIBRARY_DIRS}"
)

find_path(ODE_INCLUDE_DIR
  NAMES ode/ode.h
  HINTS "${pc_ode_INCLUDEDIR}"
)

set(ODE_VERSION "${pc_ode_VERSION}")
set(ODE_PRECISION "${pc_ode_precision}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ODE
  REQUIRED_VARS ODE_LIBRARIES ODE_INCLUDE_DIR ODE_PRECISION
  VERSION_VAR   ODE_VERSION
)

mark_as_advanced(ODE_LIBRARIES ODE_INCLUDE_DIR ODE_PRECISION)

if(ODE_LIBRARIES AND NOT TARGET ode::ode)
  add_library(ode::ode UNKNOWN IMPORTED)
  set_target_properties(ode::ode PROPERTIES
    IMPORTED_LOCATION "${ODE_LIBRARIES}"
    INTERFACE_INCLUDE_DIRECTORIES "${ODE_INCLUDE_DIR}")

  if(ODE_VERSION VERSION_LESS "0.13")
    if(ODE_PRECISION STREQUAL "dSINGLE")
      set_target_properties(ode::ode PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS "dSINGLE")
    elseif(ODE_PRECISION STREQUAL "dDOUBLE")
      set_target_properties(ode::ode PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS "dDOUBLE")
    endif()
  else()
    if(ODE_PRECISION STREQUAL "dSINGLE")
      set_target_properties(ode::ode PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS "dIDESINGLE")
    elseif(ODE_PRECISION STREQUAL "dDOUBLE")
      set_target_properties(ode::ode PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS "dIDEDOUBLE")
    endif()
  endif()
endif()
