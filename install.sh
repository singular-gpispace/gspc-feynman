#!/bin/bash

# Clean previous build and install directories
echo "🧹 Cleaning previous build and install directories..."
rm -rf ~/gpi/try_gpi/feynman_ibp/build_dir/* ~/gpi/try_gpi/feynman_ibp/install_dir/*

# Create necessary directories
mkdir -p ~/gpi/try_gpi/feynman_ibp/build_dir ~/gpi/try_gpi/feynman_ibp/install_dir

# Activate the Spack environment
echo "🚀 Loading GPI-Space environment..."
spack load gpi-space@24.12 || { echo "❌ Failed to load GPI-Space"; exit 1; }

# Set GPI-Space root
GPISPACE_ROOT="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gpi-space-24.12-jz6b4m6ql54fmhkpq6gbico2neic3kd5"
export GSPC_HOME=$GPISPACE_ROOT

# Set dependencies
FLINT_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/flint-2.6.3-pb3i4qjyjz7pqkpf6cs7wk6ro5pl564i"
GMP_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gmp-6.2.1-gjqp7e3m3fik4wsuqqcxv2brlj2wkyza"
SINGULAR_INSTALL_DIR="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-4.4.0p2-k7rgdkzo5prqsvxjckejvcdvxgjr64bk"

# Update library path
export LD_LIBRARY_PATH=$GPISPACE_ROOT/lib:$FLINT_HOME/lib:$GMP_HOME/lib:$SINGULAR_INSTALL_DIR/lib:$LD_LIBRARY_PATH

# Verify paths before building
if [[ -f $GPISPACE_ROOT/gspc_version ]]; then
    echo "✅ GPI-Space version file found!"
else
    echo "❌ GPI-Space version file is missing! Exiting..."
    exit 1
fi

# Run CMake configuration
echo "🔧 Running CMake configuration..."
cmake -D CMAKE_INSTALL_PREFIX=~/gpi/try_gpi/feynman_ibp/install_dir \
      -D CMAKE_BUILD_TYPE=Release \
      -D SINGULAR_HOME=$SINGULAR_INSTALL_DIR \
      -D Boost_NO_BOOST_CMAKE=on \
      -D GPISpace_ROOT=$GPISPACE_ROOT \
      -D GMP_HOME=$GMP_HOME \
      -D FLINT_HOME=$FLINT_HOME \
      -B ~/gpi/try_gpi/feynman_ibp/build_dir \
      -S ~/gpi/try_gpi/feynman_ibp/template || { echo "❌ CMake configuration failed!"; exit 1; }

# Build and install
echo "⚙️  Building and installing..."
cmake --build ~/gpi/try_gpi/feynman_ibp/build_dir --target install -- -j $(nproc) || { echo "❌ Build failed!"; exit 1; }

# -------------------- Copy XPNet and PNet Files --------------------
TEMPLATE_XPNET_SRC="/home/atraore/gpi/try_gpi/feynman_ibp/build_dir/workflow/template.xpnet"
DEST_WORKFLOW_DIR="/home/atraore/gpi/try_gpi/feynman_ibp/template/workflow"

# Ensure workflow directory exists
mkdir -p "$DEST_WORKFLOW_DIR"

# Copy template.xpnet to `template/workflow/`
if [[ -f "$TEMPLATE_XPNET_SRC" ]]; then
    echo "📂 Copying template.xpnet to template/workflow/..."
    cp "$TEMPLATE_XPNET_SRC" "$DEST_WORKFLOW_DIR/template.xpnet" || { echo "❌ Failed to copy template.xpnet!"; exit 1; }
    echo "✅ template.xpnet copied successfully."
else
    echo "❌ ERROR: template.xpnet not found in build_dir!"
fi

# Generate SVG workflow diagram
pnetc ~/gpi/try_gpi/feynman_ibp/template/workflow/template.xpnet | pnet2dot | dot -T svg > ~/gpi/try_gpi/feynman_ibp/template/workflow/fey.svg

echo "✅ Installation completed successfully!"
