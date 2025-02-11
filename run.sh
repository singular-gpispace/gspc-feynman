#!/bin/bash

# Navigate to example directory
cd /home/atraore/gpi/try_gpi/feynman_ibp/example_dir || exit 1
rm -rf temp/*

# Ensure only the correct version of GPI-Space is loaded
spack load gpi-space@24.12 || { echo "Failed to load GPI-Space"; exit 1; }

# Correctly set GSPC_HOME
export GSPC_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gpi-space-24.12-jz6b4m6ql54fmhkpq6gbico2neic3kd5/"


# Set Singular installation path
SINGULAR_INSTALL_DIR="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-4.4.0p2-k7rgdkzo5prqsvxjckejvcdvxgjr64bk"
export PATH=$SINGULAR_INSTALL_DIR/bin:$PATH

# Set FLINT home directory
FLINT_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/flint-2.6.3-pb3i4qjyjz7pqkpf6cs7wk6ro5pl564i"
export LD_LIBRARY_PATH=$GSPC_HOME/lib:$FLINT_HOME/lib:$LD_LIBRARY_PATH

 Singular example.sing
