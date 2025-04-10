# FindFLINT.cmake
# Try to find the FLINT library
# Once done this will define
#  FLINT_FOUND - System has FLINT
#  FLINT_INCLUDE_DIRS - The FLINT include directories
#  FLINT_LIBRARIES - The libraries needed to use FLINT

# Look for the header file
find_path(FLINT_INCLUDE_DIR 
  NAMES flint/flint.h
  HINTS 
    ENV FLINT_HOME
    ${FLINT_HOME}
  PATH_SUFFIXES include
  PATHS
    /usr/local
    /usr
)

# Look for the library
find_library(FLINT_LIBRARY
  NAMES flint
  HINTS 
    ENV FLINT_HOME
    ${FLINT_HOME}
  PATH_SUFFIXES lib
  PATHS
    /usr/local
    /usr
)

include(FindPackageHandleStandardArgs)
# Handle the QUIETLY and REQUIRED arguments and set FLINT_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(FLINT DEFAULT_MSG
  FLINT_LIBRARY FLINT_INCLUDE_DIR)

mark_as_advanced(FLINT_INCLUDE_DIR FLINT_LIBRARY)

set(FLINT_LIBRARIES ${FLINT_LIBRARY})
set(FLINT_INCLUDE_DIRS ${FLINT_INCLUDE_DIR})
set(FLINT_LIB ${FLINT_LIBRARY})
set(FLINT_INCLUDE ${FLINT_INCLUDE_DIR}) 