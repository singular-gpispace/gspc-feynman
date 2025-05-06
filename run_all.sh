#!/bin/bash

# Set up environment variables
export GSPC_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gpi-space-24.12-jz6b4m6ql54fmhkpq6gbico2neic3kd5/"
export SINGULAR_INSTALL_DIR="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-4.4.0p2-k7rgdkzo5prqsvxjckejvcdvxgjr64bk"
export FLINT_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/flint-2.6.3-pb3i4qjyjz7pqkpf6cs7wk6ro5pl564i"
export GMP_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gmp-6.2.1-gjqp7e3m3fik4wsuqqcxv2brlj2wkyza"

# Set the library path correctly
export LD_LIBRARY_PATH=$GSPC_HOME/lib:$FLINT_HOME/lib:$GMP_HOME/lib:$SINGULAR_INSTALL_DIR/lib:$LD_LIBRARY_PATH

# Load GPI-Space
spack load gpi-space@24.12 || { echo "Failed to load GPI-Space"; exit 1; }

# Run installation script
cd /home/atraore/gpi/try_gpi/feynman_ibp && bash install.sh

# Run execution script
cd /home/atraore/gpi/try_gpi/feynman_ibp && bash run.sh 