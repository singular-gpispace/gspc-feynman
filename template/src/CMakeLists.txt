# Ensure C++17 support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=1 -D_GLIBCXX_USE_FILESYSTEM=1")

# Generate installation.hpp
configure_file("installation.hpp.in" "${CMAKE_BINARY_DIR}/installation.hpp" @ONLY)

# Ensure the generated header is found
include_directories("${CMAKE_BINARY_DIR}")

# Add installation library
extended_add_library(NAME installation
  TYPE SHARED
  NAMESPACE SINGULAR-template
  SOURCES "installation.cpp"
  LIBRARIES Util::Generic GPISpace::execution ${FLINT_LIB}
  POSITION_INDEPENDENT
  INSTALL
)

set_target_properties(SINGULAR-template-installation
  PROPERTIES
  INSTALL_RPATH_USE_LINK_PATH TRUE
)

# Add module library
extended_add_library(NAME module
  TYPE SHARED
  NAMESPACE SINGULAR-template
  SOURCES "bin/SINGULAR-template-module.cpp"
  "bin/execute.cpp"
  "bin/parse_parameters_from_singular.cpp"
  "bin/ValuesOnPorts.cpp"
  "bin/Workflow.cpp"
  "bin/WorkflowResult.cpp"
  "${CMAKE_SOURCE_DIR}/template/singular_functions.cpp"
  "${CMAKE_SOURCE_DIR}/template/feynman.cpp"

  COMPILE_DEFINITIONS SING_NDEBUG OM_NDEBUG
  LIBRARIES SINGULAR-template::installation Util::Generic GPISpace::execution Singular
  POSITION_INDEPENDENT
  INSTALL
  INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
