#!/bin/bash

set -e  # Exit on error
set -o pipefail

echo "📦 Locating Spack installations..."

# Get Spack locations
SINGULAR_PATH=$(spack location -i singular@4.4.0p2)
GPISPACE_PATH=$(spack location -i gpi-space@24.12)
QT_PATH=$(spack location -i qt@5.15.12)
FLINT_PATH=$(spack location -i /pb3i4qjyjz7pqkpf6cs7wk6ro5pl564i)
GMP_PATH=$(spack location -i /gjqp7e3)
BOOST_PATH=$(spack location -i /4s3r5r7nkg2ovd454rdp4hkctg2vgfwh)

# Verify paths
for var in SINGULAR_PATH GPISPACE_PATH QT_PATH FLINT_PATH GMP_PATH BOOST_PATH; do
    if [ ! -d "${!var}" ]; then
        echo "❌ Error: Directory not found for $var (${!var})"
        exit 1
    fi
done

# Create temporary directories for copying
echo "🧹 Preparing temporary directories..."
mkdir -p temp_singular temp_gpispace temp_qt temp_flint temp_gmp temp_boost

# Copy files
echo "📁 Copying Spack installations..."
cp -a "$SINGULAR_PATH"/. temp_singular/
cp -a "$GPISPACE_PATH"/. temp_gpispace/
cp -a "$QT_PATH"/. temp_qt/
cp -a "$FLINT_PATH"/. temp_flint/
cp -a "$GMP_PATH"/. temp_gmp/
cp -a "$BOOST_PATH"/. temp_boost/

# Build Docker image
echo "🐳 Building Docker image..."
docker build \
  --build-arg SINGULAR_PATH=temp_singular \
  --build-arg GPISPACE_PATH=temp_gpispace \
  --build-arg QT_PATH=temp_qt \
  --build-arg FLINT_PATH=temp_flint \
  --build-arg GMP_PATH=temp_gmp \
  --build-arg BOOST_PATH=temp_boost \
  -t gspc-feynman .

# Cleanup
echo "🧽 Cleaning up..."
rm -rf temp_singular temp_gpispace temp_qt temp_flint temp_gmp temp_boost

echo "✅ Done! Docker image 'gspc-feynman' is ready."
