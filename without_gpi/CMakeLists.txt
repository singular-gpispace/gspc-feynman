cmake_minimum_required(VERSION 3.10)
project(FeynmanSingular)

# Set RPATH settings
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Add custom module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Print debug information
message(STATUS "Project source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Project binary dir: ${CMAKE_BINARY_DIR}")
message(STATUS "Project install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMake module path: ${CMAKE_MODULE_PATH}")

# Find dependencies
find_package(Singular REQUIRED)
find_package(FLINT REQUIRED)
find_package(GMP REQUIRED)

# Explicitly set Boost paths to Spack installation
set(BOOST_ROOT "/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/boost-1.63.0-6xz3jlconrhe7i2kz6tqvmom3huzco76")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

# Prevent CMake from looking at system Boost
set(Boost_NO_BOOST_CMAKE TRUE)
set(Boost_NO_SYSTEM_PATHS TRUE)

# Find Boost components
find_package(Boost 1.63.0 EXACT COMPONENTS filesystem system REQUIRED)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost 1.63.0 not found at ${BOOST_ROOT}. Please verify the installation.")
endif()

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost_FILESYSTEM_LIBRARY: ${Boost_FILESYSTEM_LIBRARY}")
message(STATUS "Boost_SYSTEM_LIBRARY: ${Boost_SYSTEM_LIBRARY}")

# Configure the config.hpp file
configure_file(config.hpp.in ${CMAKE_BINARY_DIR}/config.hpp @ONLY)
include_directories(${CMAKE_BINARY_DIR})

# Add singular_functions as a library
add_library(singular_functions STATIC singular_functions.cpp)
target_include_directories(singular_functions PRIVATE
    ${Singular_INCLUDE_DIRS}
    ${FLINT_INCLUDE}
    ${GMP_INCLUDE}
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(singular_functions PRIVATE
    ${Singular_LIBRARIES}
    ${FLINT_LIB}
    ${GMP_LIB}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

# Add executables
add_executable(feynman feynman.cpp)
add_executable(test_singular test_singular.cpp)

# Link libraries
target_link_libraries(feynman 
    PRIVATE 
    singular_functions
    ${Singular_LIBRARIES}
    ${FLINT_LIB}
    ${GMP_LIB}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)

target_link_libraries(test_singular
    PRIVATE
    singular_functions
    ${Singular_LIBRARIES}
    ${FLINT_LIB}
    ${GMP_LIB}
    ${Boost_FILESYSTEM_LIBRARY}
)

# Include directories
target_include_directories(feynman PRIVATE
    ${Singular_INCLUDE_DIRS}
    ${FLINT_INCLUDE}
    ${GMP_INCLUDE}
    ${Boost_INCLUDE_DIRS}
)

target_include_directories(test_singular PRIVATE
    ${Singular_INCLUDE_DIRS}
    ${FLINT_INCLUDE}
    ${GMP_INCLUDE}
    ${Boost_INCLUDE_DIRS}
)

# Install example.lib
install(FILES example.lib
    DESTINATION lib/singular
)

install(TARGETS feynman test_singular singular_functions
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)