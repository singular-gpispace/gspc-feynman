#!/bin/bash

# Clean previous build and install directories
#rm -rf ~/gpi/try_gpi/feynman_ibp/build_dir/* ~/gpi/try_gpi/feynman_ibp/install_dir/*

# Create necessary directories
mkdir -p ~/gpi/try_gpi/feynman_ibp/build_dir ~/gpi/try_gpi/feynman_ibp/install_dir

# Activate the Spack environment
spack load gpi-space@24.12

# Generate SVG workflow diagram
pnetc ~/gpi/try_gpi/feynman_ibp/template/workflow/template.xpnet | pnet2dot | dot -T svg > ~/gpi/try_gpi/feynman_ibp/template/workflow/fey.svg

# Define variables
INSTALL_PREFIX="/home/atraore/gpi/try_gpi/feynman_ibp/install_dir/"
BUILD_TYPE="Release"
BOOST_NO_CMAKE="on"

# Set GPI-Space root (for version 24.12)
GPISPACE_ROOT="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gpi-space-24.12-jz6b4m6ql54fmhkpq6gbico2neic3kd5"
# Set GPI-Space root (for version 24.12)
GPISPACE_ROOT="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gpi-space-23.06-6oiwgcz4ou6sktku324hoesngmqoiv6b/"

# Set FLINT home directory
FLINT_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/flint-2.6.3-pb3i4qjyjz7pqkpf6cs7wk6ro5pl564i"

# Set GMP home directory
GMP_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gmp-6.2.1-gjqp7e3m3fik4wsuqqcxv2brlj2wkyza"

# Set Singular install directory
SINGULAR_INSTALL_DIR="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-4.4.0p2-k7rgdkzo5prqsvxjckejvcdvxgjr64bk"

#old path
SINGULAR_INSTALL_DIR="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-snapshot_22_03-5jvwtprazqirywu2triw6rprjazzi3so/"

# Set the library path
export LD_LIBRARY_PATH=$FLINT_HOME/lib:$GMP_HOME/lib:$SINGULAR_INSTALL_DIR/lib:$LD_LIBRARY_PATH

# Run CMake
cmake -D CMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
      -D CMAKE_BUILD_TYPE=$BUILD_TYPE \
      -D SINGULAR_HOME=$SINGULAR_INSTALL_DIR \
      -D Boost_NO_BOOST_CMAKE=$BOOST_NO_CMAKE \
      -D GPISpace_ROOT=$GPISPACE_ROOT \
      -D GMP_HOME=$GMP_HOME \
      -D FLINT_HOME=$FLINT_HOME \
      -B ~/gpi/try_gpi/feynman_ibp/build_dir \
      -S ~/gpi/try_gpi/feynman_ibp/template

# Build and install
cmake --build ~/gpi/try_gpi/feynman_ibp/build_dir --target install -- -j $(nproc)
