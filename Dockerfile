# Build stage
FROM ubuntu:22.04 as builder

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    ca-certificates \
    coreutils \
    curl \
    environment-modules \
    gfortran \
    git \
    gpg \
    lsb-release \
    python3 \
    python3-distutils \
    python3-venv \
    unzip \
    zip \
    && rm -rf /var/lib/apt/lists/*

# Create working directory
WORKDIR /singular-gpispace

# Set environment variables
ENV software_ROOT=/singular-gpispace
ENV install_ROOT=/singular-gpispace

# Install Spack
RUN git clone https://github.com/spack/spack.git $software_ROOT/spack && \
    cd $software_ROOT/spack && \
    git checkout releases/v0.23 && \
    . $software_ROOT/spack/share/spack/setup-env.sh && \
    spack spec zlib

# Add spack-packages repository
RUN git clone https://github.com/singular-gpispace/spack-packages.git $software_ROOT/spack-packages && \
    . $software_ROOT/spack/share/spack/setup-env.sh && \
    spack repo add $software_ROOT/spack-packages

# Install required packages
RUN . $software_ROOT/spack/share/spack/setup-env.sh && \
    spack install gpi-space@24.12 && \
    spack install singular@4.4.0p2 && \
    spack install gspc-feynman

# Runtime stage
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    openssh-server \
    && rm -rf /var/lib/apt/lists/*

# Create working directory
WORKDIR /singular-gpispace

# Copy necessary files from builder
COPY --from=builder /singular-gpispace/spack/opt/spack/linux-ubuntu22.04-x86_64/gcc-11.3.0/gpi-space-24.12-* /singular-gpispace/gpi-space/
COPY --from=builder /singular-gpispace/spack/opt/spack/linux-ubuntu22.04-x86_64/gcc-11.3.0/singular-4.4.0p2-* /singular-gpispace/singular/
COPY --from=builder /singular-gpispace/spack/opt/spack/linux-ubuntu22.04-x86_64/gcc-11.3.0/gspc-feynman-* /singular-gpispace/gspc-feynman/

# Set up environment variables
ENV software_ROOT=/singular-gpispace
ENV install_ROOT=/singular-gpispace
ENV SINGULAR_INSTALL_DIR=/singular-gpispace/singular
ENV GSPC_FEYNMAN_INSTALL_DIR=/singular-gpispace/gspc-feynman
ENV GSPC_FEYNMAN_EXAMPLES_DIR=$GSPC_FEYNMAN_INSTALL_DIR/share/examples
ENV SINGULARPATH=$GSPC_FEYNMAN_INSTALL_DIR
ENV GPI_SPACE_HOME=/singular-gpispace/gpi-space
ENV PATH=$SINGULAR_INSTALL_DIR/bin:$GPI_SPACE_HOME/bin:$PATH
ENV LD_LIBRARY_PATH=$GPI_SPACE_HOME/lib:$GSPC_FEYNMAN_INSTALL_DIR/lib:$SINGULAR_INSTALL_DIR/lib:$LD_LIBRARY_PATH

# Create required directories and files
RUN mkdir -p $software_ROOT/tempdir && \
    hostname > $software_ROOT/nodefile && \
    hostname > $software_ROOT/loghostfile

# Set up SSH
RUN ssh-keygen -t rsa -b 4096 -N '' -f /root/.ssh/id_rsa && \
    ssh-copy-id -f -i /root/.ssh/id_rsa localhost

# Create entrypoint script
RUN echo '#!/bin/bash\n\
# Start GPI-Space monitor\n\
pkill -f gspc-monitor || true\n\
gspc-monitor --port 9876 &\n\
\n\
# Execute the command passed to docker run\n\
exec "$@"' > /entrypoint.sh && \
    chmod +x /entrypoint.sh

# Expose GPI-Space monitor port
EXPOSE 9876

# Set entrypoint
ENTRYPOINT ["/entrypoint.sh"]

# Default command
CMD ["Singular"] 