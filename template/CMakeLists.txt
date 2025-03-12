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

# Define paths
set(GENERATED_XPNET "${CMAKE_BINARY_DIR}/workflow/template.xpnet")
set(COMPILED_PETRI_NET "${CMAKE_BINARY_DIR}/template.pnet")
set(WRAPPER_DIRECTORY "${CMAKE_BINARY_DIR}/gen")
set(WRAPPER_LIBRARY "${CMAKE_SOURCE_DIR}/bin/lib/libtemplate_module.so")

# -------------------- Ensure Interface Directory is Available --------------------
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/template/interface")
file(COPY "${CMAKE_SOURCE_DIR}/interface/"
     DESTINATION "${CMAKE_BINARY_DIR}/template/interface/")

include_directories("${CMAKE_BINARY_DIR}/template/interface")

message(STATUS "Checking if interface directory exists: ${CMAKE_BINARY_DIR}/template/interface/")
execute_process(COMMAND ls -l "${CMAKE_BINARY_DIR}/template/interface/" RESULT_VARIABLE ls_result)
message(STATUS "Interface directory contents: ${ls_result}")

# -------------------- Generate Executable --------------------
add_executable(generate template/generate.cpp)
target_include_directories(generate PUBLIC 
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_BINARY_DIR}/template/interface"
)
target_link_libraries(generate PRIVATE ${FLINT_LIB})

# -------------------- Generate XPNet File --------------------
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/workflow/template.xpnet"
  COMMAND generate
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
  COMMENT "🔹 Running generate.cpp to create template.xpnet"
  VERBATIM
)

# Ensure generate_xpnet target is executed first
add_custom_target(generate_xpnet
  DEPENDS "${CMAKE_BINARY_DIR}/workflow/template.xpnet"
)

# Ensure template.pnet waits for template.xpnet
add_custom_target(generate_xpnet_and_petri_net
  ALL
  DEPENDS generate_xpnet template.pnet_and_wrapper-library
)

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
