# FindGMP.cmake
# Try to find the GMP library
# Once done this will define
#  GMP_FOUND - System has GMP
#  GMP_INCLUDE_DIRS - The GMP include directories
#  GMP_LIBRARIES - The libraries needed to use GMP

# Look for the header file
find_path(GMP_INCLUDE_DIR 
  NAMES gmp.h
  HINTS 
    ENV GMP_HOME
    ${GMP_HOME}
  PATH_SUFFIXES include
  PATHS
    /usr/local
    /usr
)

# Look for the library
find_library(GMP_LIBRARY
  NAMES gmp
  HINTS 
    ENV GMP_HOME
    ${GMP_HOME}
  PATH_SUFFIXES lib
  PATHS
    /usr/local
    /usr
)

include(FindPackageHandleStandardArgs)
# Handle the QUIETLY and REQUIRED arguments and set GMP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GMP DEFAULT_MSG
  GMP_LIBRARY GMP_INCLUDE_DIR)

mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARY)

set(GMP_LIBRARIES ${GMP_LIBRARY})
set(GMP_INCLUDE_DIRS ${GMP_INCLUDE_DIR})
set(GMP_LIB ${GMP_LIBRARY})
set(GMP_INCLUDE ${GMP_INCLUDE_DIR}) 