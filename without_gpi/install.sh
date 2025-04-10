#!/bin/bash

# Clean previous build and install directories
rm -rf ~/gpi/try_gpi/feynman_ibp/without_gpi/build_dir/* ~/gpi/try_gpi/feynman_ibp/without_gpi/install_dir/*

# Create necessary directories
mkdir -p ~/gpi/try_gpi/feynman_ibp/without_gpi/build_dir ~/gpi/try_gpi/feynman_ibp/without_gpi/install_dir

# Define variables
INSTALL_PREFIX="/home/atraore/gpi/try_gpi/feynman_ibp/without_gpi/install_dir"
BUILD_TYPE="Release"

# Set dependency paths
SINGULAR_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-4.4.0p2-k7rgdkzo5prqsvxjckejvcdvxgjr64bk"
FLINT_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/flint-2.6.3-pb3i4qjyjz7pqkpf6cs7wk6ro5pl564i"
GMP_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gmp-6.2.1-gjqp7e3m3fik4wsuqqcxv2brlj2wkyza"

# Set library path
export LD_LIBRARY_PATH=$SINGULAR_HOME/lib:$FLINT_HOME/lib:$GMP_HOME/lib:$LD_LIBRARY_PATH

# Verify paths
if [ ! -d "$SINGULAR_HOME" ]; then
  echo "Error: SINGULAR_HOME directory not found!"
  exit 1
fi
echo "Singular found at $SINGULAR_HOME"

# Run CMake
cmake -D CMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
      -D CMAKE_BUILD_TYPE=$BUILD_TYPE \
      -D SINGULAR_HOME=$SINGULAR_HOME \
      -D FLINT_HOME=$FLINT_HOME \
      -D GMP_HOME=$GMP_HOME \
      -B ~/gpi/try_gpi/feynman_ibp/without_gpi/build_dir \
      -S ~/gpi/try_gpi/feynman_ibp/without_gpi

# Build and install
cmake --build ~/gpi/try_gpi/feynman_ibp/without_gpi/build_dir --target install -- -j $(nproc)

echo "Build and installation complete. Executable at $INSTALL_PREFIX/bin/feynman"
echo "Singular library installed at $INSTALL_PREFIX/lib/singular/example.lib"