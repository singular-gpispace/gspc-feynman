#!/bin/bash

# Get Spack locations
SINGULAR_PATH=$(spack location -i singular@4.4.0p2)
GPISPACE_PATH=$(spack location -i gpi-space@24.12)
QT_PATH=$(spack location -i qt@5.15.12)
FLINT_PATH=$(spack location -i /pb3i4qjyjz7pqkpf6cs7wk6ro5pl564i)
GMP_PATH=$(spack location -i /gjqp7e3)
BOOST_PATH=$(spack location -i /4s3r5r7nkg2ovd454rdp4hkctg2vgfwh)

# Create temporary directories for copying
mkdir -p temp_singular temp_gpispace temp_qt temp_flint temp_gmp temp_boost

# Copy the files to temporary directories
cp -r "$SINGULAR_PATH"/* temp_singular/
cp -r "$GPISPACE_PATH"/* temp_gpispace/
cp -r "$QT_PATH"/* temp_qt/
cp -r "$FLINT_PATH"/* temp_flint/
cp -r "$GMP_PATH"/* temp_gmp/
cp -r "$BOOST_PATH"/* temp_boost/

# Build Docker image with build arguments
docker build \
  --build-arg SINGULAR_PATH=temp_singular \
  --build-arg GPISPACE_PATH=temp_gpispace \
  --build-arg QT_PATH=temp_qt \
  --build-arg FLINT_PATH=temp_flint \
  --build-arg GMP_PATH=temp_gmp \
  --build-arg BOOST_PATH=temp_boost \
  -t gspc-feynman .

# Clean up temporary directories
rm -rf temp_singular temp_gpispace temp_qt temp_flint temp_gmp temp_boost 