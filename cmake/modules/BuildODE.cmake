# build ODE, because some versions of it cause grSim to segfault somewhere
# could be because in some packages the double precision is turned off
include(ExternalProject)

ExternalProject_Add(ode_external
    GIT_REPOSITORY    https://bitbucket.org/odedevs/ode.git
    GIT_TAG           0.16.4
    CMAKE_ARGS
                      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
                      -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
                      -DCMAKE_C_COMPILER:PATH=${CMAKE_C_COMPILER}
                      -DCMAKE_CXX_COMPILER:PATH=${CMAKE_CXX_COMPILER}
                      -DCMAKE_MAKE_PROGRAM:PATH=${CMAKE_MAKE_PROGRAM}
                      # necessary, because it does not build the static libs if this is ON
                      -DBUILD_SHARED_LIBS=OFF
                      # if this is OFF grSim just dies instantly and INSTALL.md says it should be ON
                      -DODE_DOUBLE_PRECISION=ON
                      -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  STEP_TARGETS install
)

set(ODE_LIB_SUBPATH "${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}ode${CMAKE_STATIC_LIBRARY_SUFFIX}")

# the byproducts are available after the install step
ExternalProject_Add_Step(ode_external out
  DEPENDEES install
  BYPRODUCTS
      "<INSTALL_DIR>/${ODE_LIB_SUBPATH}"
)

ExternalProject_Get_Property(ode_external install_dir)
set(ODE_LIBRARY "${install_dir}/${ODE_LIB_SUBPATH}")
list(APPEND libs ${ODE_LIBRARY})
target_include_directories(${app} PRIVATE "${install_dir}/include")
