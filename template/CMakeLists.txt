cmake_minimum_required(VERSION 3.16)
project(singular-template LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

set(INSTALL_DO_NOT_BUNDLE ON CACHE BOOL "")

find_package(GPISpace REQUIRED)

find_package(util-generic REQUIRED)
find_package(util-cmake REQUIRED)

include(util-cmake/add_macros)

include(util-cmake/beautify_find_boost)
include(util-cmake/beautify_find_GPISpace)

find_boost(1.61 REQUIRED QUIET FROM_GPISPACE_INSTALLATION COMPONENTS
  date_time
  filesystem
  program_options
  regex
  serialization
  system
  test_exec_monitor
  thread
  unit_test_framework
  timer
  iostreams
  chrono
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
find_package(Singular REQUIRED)

# FLINT include and library directories
# set(FLINT_INCLUDE_DIR "/usr/local/include/flint")
# set(FLINT_LIB_DIR "/usr/local/lib")

# Add FLINT include directory
# include_directories(${FLINT_INCLUDE_DIR})

# FLINT library search
# find_library(FLINT_LIB flint HINTS "${FLINT_LIB_DIR}")

# Check if FLINT library was found
#[[ if(NOT FLINT_LIB)
  message(FATAL_ERROR "FLINT library not found!")
endif()

# Link FLINT library directory
link_directories(${FLINT_LIB_DIR})
]]
# Include directories for project
include_directories("src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/src")

# Add subdirectories
add_subdirectory(src)
add_subdirectory(template)
add_subdirectory(workflow)

# Configure config.hpp file
configure_file("template/config.hpp.in"
  "${CMAKE_BINARY_DIR}/template/config.hpp"
  @ONLY
)

# Add custom target to ensure config.hpp is generated
add_custom_target(Config
  ALL
  DEPENDS "${CMAKE_BINARY_DIR}/template/config.hpp"
)

# Install the examples directory
# install(DIRECTORY examples
# DESTINATION "${CMAKE_INSTALL_PREFIX}/share"
# )

# Link the FLINT library to the  target  template_implementation
# target_link_libraries(template_implementation PRIVATE ${FLINT_LIB})