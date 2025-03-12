#!/bin/bash

# Navigate to example directory
cd /home/atraore/gpi/try_gpi/feynman_ibp/example_dir || exit 1
rm -rf temp/*

# Ensure only the correct version of GPI-Space is loaded
spack load gpi-space@24.12 || { echo "❌ Failed to load GPI-Space"; exit 1; }

# Set environment variables
export GSPC_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gpi-space-24.12-jz6b4m6ql54fmhkpq6gbico2neic3kd5/"

# Set Singular installation path
SINGULAR_INSTALL_DIR="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-4.4.0p2-k7rgdkzo5prqsvxjckejvcdvxgjr64bk"
export PATH=$SINGULAR_INSTALL_DIR/bin:$PATH

# Set FLINT home directory
FLINT_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/flint-2.6.3-pb3i4qjyjz7pqkpf6cs7wk6ro5pl564i"
export LD_LIBRARY_PATH=$GSPC_HOME/lib:$FLINT_HOME/lib:$LD_LIBRARY_PATH

# Define paths
TEMPLATE_PNET_SOURCE="/home/atraore/gpi/try_gpi/feynman_ibp/build_dir/template.pnet"
TEMPLATE_PNET_DEST="/home/atraore/gpi/try_gpi/feynman_ibp/install_dir/libexec/workflow/template.pnet"

# Ensure template.pnet exists, copy if missing
if [ ! -f "$TEMPLATE_PNET_DEST" ]; then
    echo "⚠️  template.pnet not found in install directory. Copying from build_dir..."
    mkdir -p "$(dirname "$TEMPLATE_PNET_DEST")"  # Ensure the directory exists
    cp "$TEMPLATE_PNET_SOURCE" "$TEMPLATE_PNET_DEST" || { echo "❌ Failed to copy template.pnet!"; exit 1; }
    echo "✅ template.pnet copied successfully."
fi

# Run Singular
Singular example.sing
