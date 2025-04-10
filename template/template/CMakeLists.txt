# Add the FLINT library to the linking process
extended_add_library(NAME template_implementation
  TYPE MODULE
  SOURCES "implementation.cpp" "singular_functions.cpp"
  INCLUDE_DIRECTORIES PRIVATE
  "${CMAKE_BINARY_DIR}"
  "${CMAKE_BINARY_DIR}/gen"
  "${CMAKE_BINARY_DIR}/template"

  # "${FLINT_INCLUDE_DIR}" # Add FLINT include directory
  COMPILE_DEFINITIONS SING_NDEBUG OM_NDEBUG
  LIBRARIES GPISpace::workflow_development Singular # ${FLINT_LIB} # Link FLINT
  DEPENDS template.pnet_and_wrapper-library Config
  INSTALL INSTALL_DESTINATION "libexec/workflow"
)

#[[
include_directories("${FLINT_HOME}/include/flint")

set(FLINT_LIB "/usr/local/lib/libflint.so")

# Example to link FLINT and other libraries
target_link_libraries(template_implementation
  PRIVATE
  ${FLINT_LIB} # FLINT library
  ${SINGULAR_LIB} # Singular library, if necessary
)
]]
# Set the properties for the installed library
set_target_properties(template_implementation
  PROPERTIES
  INSTALL_RPATH_USE_LINK_PATH TRUE
)

# Install the templategspc.lib file
# install(FILES LIB/templategspc.lib
# DESTINATION "${CMAKE_INSTALL_PREFIX}"
# )