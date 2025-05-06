# Ensure SINGULAR_HOME is set
if(NOT DEFINED SINGULAR_HOME AND DEFINED ENV{SINGULAR_HOME})
  set(SINGULAR_HOME $ENV{SINGULAR_HOME})
endif()

# Find Singular Paths
find_path(SINGULAR_HOME
  NAMES "lib/libSingular.so" "include/singular/Singular/libsingular.h"
  HINTS ${SINGULAR_HOME} ENV SINGULAR_HOME
)

find_library(SINGULAR_LIBRARY
  NAMES "Singular"
  HINTS ${SINGULAR_HOME}
  PATH_SUFFIXES "lib"
)

find_library(OMALLOC_LIBRARY
  NAMES "omalloc"
  HINTS ${SINGULAR_HOME}
  PATH_SUFFIXES "lib"
)

find_library(POLYS_LIBRARY
  NAMES "polys"
  HINTS ${SINGULAR_HOME}
  PATH_SUFFIXES "lib"
)

find_library(RESOURCES_LIBRARY
  NAMES "singular_resources"
  HINTS ${SINGULAR_HOME}
  PATH_SUFFIXES "lib"
)

find_path(SINGULAR_INCLUDE_DIR
  NAMES "singular/Singular/libsingular.h"
  HINTS ${SINGULAR_HOME}
  PATH_SUFFIXES "include"
)

find_file(SINGULAR_CONFIG_BIN
  NAMES "libsingular-config"
  HINTS ${SINGULAR_HOME}
  PATH_SUFFIXES "bin"
)

# Find GMP (Singular requires it)
find_path(GMP_INCLUDE_DIR
  NAMES gmp.h
  PATH_SUFFIXES "include"
  HINTS ${GMP_HOME} ENV GMP_HOME
)

find_library(GMP_LIBRARY
  NAMES "gmp"
  HINTS ${GMP_HOME} ENV GMP_HOME
)

# Find FLINT Paths
find_path(FLINT_HOME
  NAMES "lib/libflint.so"
  HINTS ${FLINT_HOME} ENV FLINT_HOME
)

find_library(FLINT_LIBRARY
  NAMES "flint"
  HINTS ${FLINT_HOME}
  PATH_SUFFIXES "lib"
)

# Mark advanced variables
mark_as_advanced(SINGULAR_HOME
  SINGULAR_LIBRARY
  OMALLOC_LIBRARY
  POLYS_LIBRARY
  RESOURCES_LIBRARY
  SINGULAR_INCLUDE_DIR
  SINGULAR_CONFIG_BIN
  GMP_INCLUDE_DIR
  GMP_LIBRARY
  FLINT_HOME
  FLINT_LIBRARY
  SINGULAR_VERSION
)

# Find package standard args
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Singular
  REQUIRED_VARS SINGULAR_HOME
  SINGULAR_LIBRARY
  OMALLOC_LIBRARY
  POLYS_LIBRARY
  RESOURCES_LIBRARY
  SINGULAR_INCLUDE_DIR
  SINGULAR_CONFIG_BIN
  GMP_INCLUDE_DIR
  GMP_LIBRARY
  FLINT_HOME
  FLINT_LIBRARY
)

# Add Singular as a CMake library
if(Singular_FOUND)
  extended_add_library(NAME Singular
    SYSTEM_INCLUDE_DIRECTORIES INTERFACE
    "${SINGULAR_INCLUDE_DIR}"
    "${SINGULAR_INCLUDE_DIR}/singular"
    "${GMP_INCLUDE_DIR}"
    "${FLINT_HOME}/include"
    LIBRARIES ${SINGULAR_LIBRARY}
    ${FLINT_LIBRARY} 
    ${OMALLOC_LIBRARY}
    ${POLYS_LIBRARY}
    ${RESOURCES_LIBRARY}
    ${GMP_LIBRARY}
  )
endif()
