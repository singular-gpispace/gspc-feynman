#!/bin/bash

# Navigate to project directory
cd /home/atraore/gpi/try_gpi/feynman_ibp/without_gpi || { echo "Failed to change directory"; exit 1; }

# Define paths to dependencies
SINGULAR_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-4.4.0p2-k7rgdkzo5prqsvxjckejvcdvxgjr64bk"
FLINT_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/flint-2.6.3-pb3i4qjyjz7pqkpf6cs7wk6ro5pl564i"
GMP_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gmp-6.2.1-gjqp7e3m3fik4wsuqqcxv2brlj2wkyza"
BOOST_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/boost-1.63.0-6xz3jlconrhe7i2kz6tqvmom3huzco76"

# Set up environment variables
export PATH="${SINGULAR_HOME}/bin:${PATH}"
export LD_LIBRARY_PATH="${SINGULAR_HOME}/lib:${FLINT_HOME}/lib:${GMP_HOME}/lib:${BOOST_HOME}/lib:${LD_LIBRARY_PATH}"

# Define the executable path
EXECUTABLE="install_dir/bin/feynman"

# Check if the executable exists
if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Executable $EXECUTABLE not found. Please build and install the project first."
    exit 1
fi

# Ensure example.lib is in the install directory
if [ ! -f "install_dir/lib/singular/example.lib" ]; then
    echo "Error: example.lib not found in install_dir/lib/singular. Please ensure it's installed."
    exit 1
fi

# Run the feynman executable
echo "Running Feynman with Singular procedure from example.lib..."
./"$EXECUTABLE"

# Check the exit status
if [ $? -eq 0 ]; then
    echo "Execution completed successfully."
else
    echo "Error: Execution failed with exit code $?"
    exit 1
fi