#!/bin/bash

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
WORKSPACE_DIR="$SCRIPT_DIR"

# Clean previous build and install directories
rm -rf "$WORKSPACE_DIR/build_dir"/* "$WORKSPACE_DIR/install_dir"/*

# Create necessary directories with proper permissions
mkdir -p "$WORKSPACE_DIR/build_dir" "$WORKSPACE_DIR/install_dir/share/examples/"
mkdir -p "$WORKSPACE_DIR/install_dir/temp/"
chmod 755 "$WORKSPACE_DIR/install_dir/temp/"

# copy example files from examples to the install_dir
cp -r "$WORKSPACE_DIR/examples/templategp.lib" \
      "$WORKSPACE_DIR/examples/templategspc.lib" \
      "$WORKSPACE_DIR/install_dir/share/examples/"

# Activate the Spack environment
spack load gpi-space@24.12 

# Get GPI-Space root from spack
GPISPACE_ROOT=$(spack location -i gpi-space@24.12)


# Set and verify GSPC_HOME
export GSPC_HOME="$GPISPACE_ROOT"


# Generate SVG workflow diagram
pnetc "$WORKSPACE_DIR/template/workflow/template.xpnet" | pnet2dot | dot -T svg > "$WORKSPACE_DIR/template/workflow/fey.svg"

# Define variables
INSTALL_PREFIX="$WORKSPACE_DIR/install_dir"
BUILD_TYPE="Release"
BOOST_NO_CMAKE="on"

# Get dependency paths from spack using specific hashes
FLINT_HOME=$(spack location -i /pb3i4qj)  # flint@2.6.3%gcc@11.3.0
GMP_HOME=$(spack location -i /gjqp7e3)    # gmp@6.2.1%gcc@11.3.0
SINGULAR_INSTALL_DIR=$(spack location -i singular@4.4.0p2)

# Set the library path correctly
export LD_LIBRARY_PATH=$GSPC_HOME/lib:$FLINT_HOME/lib:$GMP_HOME/lib:$SINGULAR_INSTALL_DIR/lib:$LD_LIBRARY_PATH

# Run CMake
cmake -D CMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
      -D CMAKE_BUILD_TYPE=$BUILD_TYPE \
      -D SINGULAR_HOME=$SINGULAR_INSTALL_DIR \
      -D Boost_NO_BOOST_CMAKE=$BOOST_NO_CMAKE \
      -D GPISpace_ROOT=$GSPC_HOME \
      -D GMP_HOME=$GMP_HOME \
      -D FLINT_HOME=$FLINT_HOME \
      -B "$WORKSPACE_DIR/build_dir" \
      -S "$WORKSPACE_DIR/template"

# Build and install
cmake --build "$WORKSPACE_DIR/build_dir" --target install -- -j $(nproc)

# Copy the installation library after building
cp "$WORKSPACE_DIR/build_dir/src/libSINGULAR-template-installation.so" \
   "$WORKSPACE_DIR/install_dir/"

# Navigate to example directory
cd "$WORKSPACE_DIR/examples" || exit 1
rm -rf temp/*

# Set Singular installation path
export PATH=$SINGULAR_INSTALL_DIR/bin:$PATH

# Set GSPC_FEYNMAN_INSTALL_DIR environment variable
export GSPC_FEYNMAN_INSTALL_DIR="$WORKSPACE_DIR/install_dir"

# Update LD_LIBRARY_PATH to include the install directory
export LD_LIBRARY_PATH=$GSPC_HOME/lib:$FLINT_HOME/lib:$GSPC_FEYNMAN_INSTALL_DIR:$LD_LIBRARY_PATH

# Run Singular
Singular example.lib