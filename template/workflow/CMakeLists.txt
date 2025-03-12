function(add_petrinet in out module_name)
  add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/${module_name}.pnet"
    "${CMAKE_BINARY_DIR}/gen_${out}/pnetc/op/libsingular_${module_name}.so"
    COMMAND "${GSPC_HOME}/bin/pnetc"
    --Windependent-transition=0
    --gen-cxxflags=-O3
    --gen-cxxflags=-std=c++17
    --gen-cxxflags=-I"${CMAKE_BINARY_DIR}"
    --gen-cxxflags=-I"${CMAKE_BINARY_DIR}/template"
    --gen-cxxflags=-I="${CMAKE_SOURCE_DIR}/src"
    --gen-ldflags="-L${CMAKE_BINARY_DIR}"
    --gen-ldflags="-Wl,-rpath='\\$\$\\$\$ORIGIN'"
    --force-overwrite-file=true
    --input="${CMAKE_BINARY_DIR}/workflow/template.xpnet"
    --output="${CMAKE_BINARY_DIR}/${out}"
    --path-to-cpp="${CMAKE_BINARY_DIR}/gen_${out}"
    --Wbackup-file=OFF
    --Woverwrite-file=OFF
    &&
    make -C "${CMAKE_BINARY_DIR}/gen_${out}"
    DEPENDS generate_xpnet # 👈 Ensure it waits for template.xpnet
  )

  add_custom_target("${out}_and_wrapper-library"
    ALL
    DEPENDS "${CMAKE_BINARY_DIR}/gen_${out}/pnetc/op/libsingular_${module_name}.so"
    "${CMAKE_BINARY_DIR}/${out}"
  )
endfunction()

add_petrinet(template.xpnet template.pnet template)
