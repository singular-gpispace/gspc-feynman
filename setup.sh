#!/bin/bash

# Exit on error
set -e

# Set up base directories
export software_ROOT=~/singular-gpispace
export install_ROOT=~/singular-gpispace

# Create necessary directories
mkdir -p $software_ROOT
mkdir -p $software_ROOT/tempdir

# Load required modules with specific versions
spack load gpi-space@24.12
spack load gspc-feynman

# Set up environment variables
# Use the specific Singular version required by gspc-feynman (4.4.0p2)
export SINGULAR_INSTALL_DIR=$(spack location -i singular@4.4.0p2)
export GSPC_FEYNMAN_INSTALL_DIR=$(spack location -i gspc-feynman)
export GSPC_FEYNMAN_EXAMPLES_DIR=$GSPC_FEYNMAN_INSTALL_DIR/share/examples
export SINGULARPATH=$GSPC_FEYNMAN_INSTALL_DIR
export GPI_SPACE_HOME=$(spack location -i gpi-space@24.12)

# Create required files
hostname > $software_ROOT/nodefile
hostname > $software_ROOT/loghostfile

# Copy example files
cp $GSPC_FEYNMAN_EXAMPLES_DIR/templategp.lib $software_ROOT/
cp $GSPC_FEYNMAN_EXAMPLES_DIR/templategspc.lib $software_ROOT/

# Set up SSH if not already configured
if [ ! -f ~/.ssh/id_rsa ]; then
    ssh-keygen -t rsa -b 4096 -N '' -f ~/.ssh/id_rsa
    ssh-copy-id -f -i ~/.ssh/id_rsa localhost
fi

# Start GPI-Space monitor if not already running
if ! pgrep -x "gspc-monitor" > /dev/null; then
    # Start monitor in background and disown it to prevent it from being affected by terminal closure
    gspc-monitor --port 9876 &
    MONITOR_PID=$!
    disown $MONITOR_PID
    echo "Started GPI-Space monitor on port 9876 (PID: $MONITOR_PID)"
else
    echo "GPI-Space monitor is already running"
fi

# Print status
echo "Setup completed successfully!"
echo "Environment variables:"
echo "SINGULAR_INSTALL_DIR: $SINGULAR_INSTALL_DIR"
echo "GSPC_FEYNMAN_INSTALL_DIR: $GSPC_FEYNMAN_INSTALL_DIR"
echo "GSPC_FEYNMAN_EXAMPLES_DIR: $GSPC_FEYNMAN_EXAMPLES_DIR"
echo "SINGULARPATH: $SINGULARPATH"
echo "GPI_SPACE_HOME: $GPI_SPACE_HOME"

# Instructions for running Singular
echo -e "\nTo start Singular, run:"
echo "cd $software_ROOT"
echo "Singular"

# Change to the working directory
cd $software_ROOT