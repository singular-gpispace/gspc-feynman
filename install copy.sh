#!/bin/bash

# Clean previous build and install directories
rm -rf ~/gpi/try_gpi/gspc-feynman/build_dir/* ~/gpi/try_gpi/gspc-feynman/install_dir/*

# Create necessary directories
#mkdir -p ~/gpi/try_gpi/gspc-feynman/build_dir ~/gpi/try_gpi/gspc-feynman/install_dir

# copy example files from examples to the  install_dir
mkdir -p ~/gpi/try_gpi/gspc-feynman/install_dir/share/examples/

cp -r ~/gpi/try_gpi/gspc-feynman/examples/templategp.lib \
      ~/gpi/try_gpi/gspc-feynman/examples/templategspc.lib \
      ~/gpi/try_gpi/gspc-feynman/install_dir/share/examples/

# Activate the Spack environment
spack load gpi-space@24.12 
# Generate SVG workflow diagram
pnetc ~/gpi/try_gpi/gspc-feynman/template/workflow/template.xpnet | pnet2dot | dot -T svg > ~/gpi/try_gpi/gspc-feynman/template/workflow/fey.svg

# Define variables
INSTALL_PREFIX="/home/atraore/gpi/try_gpi/gspc-feynman/install_dir/"
BUILD_TYPE="Release"
BOOST_NO_CMAKE="on"

# Set GPI-Space root
GPISPACE_ROOT="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gpi-space-24.12-jyime7tbwhdfot7bji3vqhdtaxt26qwl"
export GSPC_HOME=$GPISPACE_ROOT

# Set FLINT home directory
FLINT_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/flint-2.6.3-pb3i4qjyjz7pqkpf6cs7wk6ro5pl564i"

# Set GMP home directory
GMP_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gmp-6.2.1-gjqp7e3m3fik4wsuqqcxv2brlj2wkyza"

# Set Singular install
SINGULAR_INSTALL_DIR="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-4.4.0p2-k7rgdkzo5prqsvxjckejvcdvxgjr64bk"

# Set the library path correctly
export LD_LIBRARY_PATH=$GPISPACE_ROOT/lib:$FLINT_HOME/lib:$GMP_HOME/lib:$SINGULAR_INSTALL_DIR/lib:$LD_LIBRARY_PATH

# Verify paths before building
ls $GPISPACE_ROOT/gspc_version && echo "GPI-Space version is here!" || { echo "GPI-Space version file missing!"; exit 1; }

# Run CMake
cmake -D CMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
      -D CMAKE_BUILD_TYPE=$BUILD_TYPE \
      -D SINGULAR_HOME=$SINGULAR_INSTALL_DIR \
      -D Boost_NO_BOOST_CMAKE=$BOOST_NO_CMAKE \
      -D GPISpace_ROOT=$GPISPACE_ROOT \
      -D GMP_HOME=$GMP_HOME \
      -D FLINT_HOME=$FLINT_HOME \
      -B ~/gpi/try_gpi/gspc-feynman/build_dir \
      -S ~/gpi/try_gpi/gspc-feynman/template

# Build and install
cmake --build ~/gpi/try_gpi/gspc-feynman/build_dir --target install -- -j $(nproc)


#!/bin/bash

# Navigate to example directory
cd /home/atraore/gpi/try_gpi/gspc-feynman/examples || exit 1
rm -rf temp/*

# Set Singular installation path
export PATH=$SINGULAR_INSTALL_DIR/bin:$PATH

export LD_LIBRARY_PATH=$GSPC_HOME/lib:$FLINT_HOME/lib:$LD_LIBRARY_PATH

# Set GSPC_FEYNMAN_INSTALL_DIR environment variable
export GSPC_FEYNMAN_INSTALL_DIR="/home/atraore/gpi/try_gpi/gspc-feynman/install_dir"

Singular example.lib