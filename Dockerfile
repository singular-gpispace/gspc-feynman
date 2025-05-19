# syntax=docker/dockerfile:1.4

########################
# Build stage
########################
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential ca-certificates coreutils curl environment-modules gfortran git gpg \
    lsb-release python3 python3-distutils python3-venv unzip zip autoconf automake \
    libtool m4 pkg-config libgmp-dev libmpfr-dev libcdd-dev vim cmake libssh2-1-dev \
    libntl-dev libharfbuzz0b apt-utils libicu70 libicu-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /singular-gpispace
ENV software_ROOT=/singular-gpispace
ENV install_ROOT=/singular-gpispace

RUN mkdir -p /singular-gpispace/.spack

RUN git clone https://github.com/spack/spack.git $software_ROOT/spack && \
    cd $software_ROOT/spack && git checkout releases/v0.23

RUN git clone https://github.com/singular-gpispace/spack-packages.git $software_ROOT/spack-packages

RUN grep -q 'version("24.12"' $software_ROOT/spack/var/spack/repos/builtin/packages/gpi-space/package.py || \
    echo '    version("24.12", sha256="9cd97b8e41b4494c14a90afff6b801f9cf3b5811205e39c33a481ab09db59920")' >> \
         $software_ROOT/spack/var/spack/repos/builtin/packages/gpi-space/package.py

RUN --mount=type=cache,target=/singular-gpispace/.spack \
    bash -c 'source $software_ROOT/spack/share/spack/setup-env.sh && \
             spack repo add $software_ROOT/spack-packages'

RUN --mount=type=cache,target=/singular-gpispace/.spack \
    bash -c 'source $software_ROOT/spack/share/spack/setup-env.sh && \
             spack install gmp@6.2.1%gcc@11.4.0 && \
             spack install flint@2.6.3%gcc@11.4.0 && \
             spack install gpi-space@24.12%gcc@11.4.0 && \
             spack install singular@4.4.0p2%gcc@11.4.0 && \
             spack install gspc-feynman%gcc@11.4.0'

RUN --mount=type=cache,target=/singular-gpispace/.spack \
    bash -c 'source $software_ROOT/spack/share/spack/setup-env.sh && \
             KEEP_HASH=$(spack find --format "{hash}" gpi-space@24.12%gcc@11.4.0 | head -n 1) && \
             for h in $(spack find --format "{hash}" gpi-space@24.12%gcc@11.4.0); do \
                 if [ "$h" != "$KEEP_HASH" ]; then spack uninstall -y /$h || true; fi; \
             done'

RUN --mount=type=cache,target=/singular-gpispace/.spack \
    bash -c 'source $software_ROOT/spack/share/spack/setup-env.sh && \
             echo "GPI_HASH=$(spack find --format \"{hash}\" gpi-space@24.12%gcc@11.4.0)" > pkg_hashes.txt && \
             echo "SINGULAR_HASH=$(spack find --format \"{hash}\" singular@4.4.0p2%gcc@11.4.0)" >> pkg_hashes.txt && \
             echo "FEYNMAN_HASH=$(spack find --format \"{hash}\" gspc-feynman%gcc@11.4.0)" >> pkg_hashes.txt && \
             echo "FLINT_HASH=$(spack find --format \"{hash}\" flint@2.6.3%gcc@11.4.0)" >> pkg_hashes.txt && \
             echo "GMP_HASH=$(spack find --format \"{hash}\" gmp@6.2.1%gcc@11.4.0)" >> pkg_hashes.txt'

########################
# Runtime stage
########################
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    openssh-server libgmp-dev libmpfr-dev libcdd-dev build-essential git cmake \
    libssh2-1-dev libntl-dev libharfbuzz0b apt-utils libicu70 libicu-dev x11-apps \
    xauth libcap2-bin libxcb1 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 \
    libxcb-randr0 libxcb-render-util0 libxcb-shape0 libxcb-sync1 libxcb-xfixes0 \
    libxcb-xinerama0 libxcb-xkb1 libxkbcommon-x11-0 iproute2 && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /singular-gpispace
ENV software_ROOT=/singular-gpispace
ENV install_ROOT=/singular-gpispace

COPY --from=builder /singular-gpispace/spack /singular-gpispace/spack
COPY --from=builder /singular-gpispace/spack-packages /singular-gpispace/spack-packages
COPY --from=builder /singular-gpispace/.spack /singular-gpispace/.spack
COPY --from=builder /singular-gpispace/pkg_hashes.txt /singular-gpispace/pkg_hashes.txt

RUN mkdir -p $software_ROOT/tempdir && \
    hostname > $software_ROOT/nodefile && \
    hostname > $software_ROOT/loghostfile

RUN echo 'source /singular-gpispace/spack/share/spack/setup-env.sh' >> /etc/profile.d/spack.sh

RUN mkdir -p /var/run/sshd && \
    echo 'root:root' | chpasswd && \
    sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
    sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config && \
    ssh-keygen -t rsa -b 4096 -N '' -f /root/.ssh/id_rsa && \
    cat /root/.ssh/id_rsa.pub > /root/.ssh/authorized_keys && \
    chmod 600 /root/.ssh/authorized_keys

RUN setcap cap_sys_admin+ep /bin/hostname

RUN printf '#!/bin/bash\n\
service ssh start\n\
# Get the hostname from the container environment\n\
CONTAINER_HOSTNAME=$(hostname)\n\
CONTAINER_IP=$(ip addr show eth0 | grep "inet\b" | awk "{print \$2}" | cut -d/ -f1 | tr -d " ")\n\
source /singular-gpispace/spack/share/spack/setup-env.sh\n\
if ! spack repo list | grep -q "/singular-gpispace/spack-packages"; then\n\
    spack repo add /singular-gpispace/spack-packages\n\
fi\n\
\n\
spack load gpi-space\n\
spack load singular\n\
spack load gspc-feynman\n\
spack load flint\n\
spack load gmp\n\
\n\
export SINGULAR_INSTALL_DIR=$(spack location -i singular)\n\
export GSPC_FEYNMAN_INSTALL_DIR=$(spack location -i gspc-feynman)\n\
export GSPC_FEYNMAN_EXAMPLES_DIR=$GSPC_FEYNMAN_INSTALL_DIR/share/examples\n\
export SINGULARPATH=$GSPC_FEYNMAN_INSTALL_DIR\n\
export GPI_SPACE_HOME=$(spack location -i gpi-space)\n\
export GSPC_HOME=$GPI_SPACE_HOME\n\
export FLINT_HOME=$(spack location -i flint)\n\
export GMP_HOME=$(spack location -i gmp)\n\
export LD_LIBRARY_PATH=$GPI_SPACE_HOME/lib:$GSPC_FEYNMAN_INSTALL_DIR/lib:$SINGULAR_INSTALL_DIR/lib:$FLINT_HOME/lib:$GMP_HOME/lib:$LD_LIBRARY_PATH\n\
export PATH=$SINGULAR_INSTALL_DIR/bin:$GSPC_FEYNMAN_INSTALL_DIR/bin:$PATH\n\
export DISPLAY=:0\n\
export software_ROOT=/singular-gpispace\n\
export GPISPACE_BIN=$GPI_SPACE_HOME/bin\n\
\n\
mkdir -p $software_ROOT/logs\n\
mkdir -p $software_ROOT/tempdir\n\
\n\
cp $GSPC_FEYNMAN_INSTALL_DIR/lib/libSINGULAR-template-module.so $GSPC_FEYNMAN_INSTALL_DIR/ 2>/dev/null || true\n\
cp $GSPC_FEYNMAN_EXAMPLES_DIR/templategp.lib . 2>/dev/null || true\n\
cp $GSPC_FEYNMAN_EXAMPLES_DIR/templategspc.lib . 2>/dev/null || true\n\
\n\
# Write only hostname to nodefile and loghostfile\n\
echo "$CONTAINER_HOSTNAME" > nodefile\n\
echo "$CONTAINER_HOSTNAME" > loghostfile\n\
\n\
# Create a default Singular configuration file with SSH username and entry point\n\
mkdir -p ~/.singular\n\
echo "rifstrategyparameters = \"--ssh-username=root --entry-point=$CONTAINER_HOSTNAME\"" > ~/.singular/gspc.conf\n\
\n\
# Create a Singular startup file to ensure configuration is loaded\n\
echo "LIB \"templategspc.lib\";" > ~/.singularrc\n\
\n\
$GPISPACE_BIN/gspc-logging-to-stdout.exe --port 9876 >> $software_ROOT/logs/monitor.txt 2>&1 &\n\
\n\
echo -e "\\nWelcome to the gspc-feynman Docker container!"\n\
echo "To start Singular, run:"\n\
echo "  Singular"\n\
cd /singular-gpispace\n\
exec bash\n' > /entrypoint.sh && chmod +x /entrypoint.sh

EXPOSE 9876
ENTRYPOINT ["/entrypoint.sh"]
CMD ["bash"]
