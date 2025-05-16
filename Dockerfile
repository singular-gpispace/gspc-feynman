# ---------- Build Stage ----------
    FROM ubuntu:22.04 as builder

    ENV DEBIAN_FRONTEND=noninteractive
    
    # Build arguments for copying prebuilt packages
    ARG SINGULAR_PATH
    ARG GPISPACE_PATH
    ARG QT_PATH
    ARG FLINT_PATH
    ARG GMP_PATH
    ARG BOOST_PATH
    
    # Install build dependencies
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
        autoconf \
        automake \
        libtool \
        m4 \
        pkg-config \
        libgmp-dev \
        libmpfr-dev \
        libcdd-dev \
        vim \
        && rm -rf /var/lib/apt/lists/*
    
    WORKDIR /singular-gpispace
    
    ENV software_ROOT=/singular-gpispace
    ENV install_ROOT=/singular-gpispace
    
    # Copy preinstalled packages
    COPY $SINGULAR_PATH/ /singular-gpispace/singular/
    COPY $GPISPACE_PATH/ /singular-gpispace/gpi-space/
    COPY $QT_PATH/ /singular-gpispace/qt/
    COPY $FLINT_PATH/ /singular-gpispace/flint/
    COPY $GMP_PATH/ /singular-gpispace/gmp/
    COPY $BOOST_PATH/ /singular-gpispace/boost/
    
    
    # ---------- Runtime Stage ----------
    FROM ubuntu:22.04
    
    ENV DEBIAN_FRONTEND=noninteractive
    
    # Install runtime dependencies
    RUN apt-get update && apt-get install -y \
        openssh-server \
        libgmp-dev \
        libmpfr-dev \
        libcdd-dev \
        build-essential \
        git \
        cmake \
        libssh2-1-dev \
        libntl-dev \
        libharfbuzz0b \
        apt-utils \
        libicu70 \
        libicu-dev \
        && rm -rf /var/lib/apt/lists/*
    
    WORKDIR /singular-gpispace
    
    # Copy from builder
    COPY --from=builder /singular-gpispace /singular-gpispace
    
    # Set environment variables
    ENV software_ROOT=/singular-gpispace
    ENV install_ROOT=/singular-gpispace
    ENV SINGULAR_INSTALL_DIR=$software_ROOT/singular
    ENV GPI_SPACE_HOME=$software_ROOT/gpi-space
    ENV GSPC_HOME=$software_ROOT/gpi-space
    ENV QT5_DIR=$software_ROOT/qt
    ENV FLINT_HOME=$software_ROOT/flint
    ENV GMP_HOME=$software_ROOT/gmp
    ENV BOOST_ROOT=$software_ROOT/boost
    ENV GSPC_FEYNMAN_INSTALL_DIR=$software_ROOT/gspc-feynman
    ENV PATH=$SINGULAR_INSTALL_DIR/bin:$GPI_SPACE_HOME/bin:$QT5_DIR/bin:$FLINT_HOME/bin:$GMP_HOME/bin:$BOOST_ROOT/bin:$GSPC_FEYNMAN_INSTALL_DIR/bin:$PATH
    ENV LD_LIBRARY_PATH=$GPI_SPACE_HOME/lib:$SINGULAR_INSTALL_DIR/lib:$QT5_DIR/lib:$FLINT_HOME/lib:$GMP_HOME/lib:$BOOST_ROOT/lib:$GSPC_FEYNMAN_INSTALL_DIR/lib:$LD_LIBRARY_PATH
    ENV CMAKE_PREFIX_PATH=$QT5_DIR:$FLINT_HOME:$GMP_HOME:$BOOST_ROOT:$CMAKE_PREFIX_PATH
    ENV C_INCLUDE_PATH=$FLINT_HOME/include:$C_INCLUDE_PATH
    ENV CPLUS_INCLUDE_PATH=$FLINT_HOME/include:$CPLUS_INCLUDE_PATH
    ENV SINGULARPATH=$GSPC_FEYNMAN_INSTALL_DIR
    ENV PATH=$PATH:/singular-gpispace/gpi-space/libexec/gspc:/singular-gpispace/gpi-space/libexec/bundle/info
    # Clone and build gspc-feynman
    RUN git clone https://github.com/singular-gpispace/gspc-feynman.git $software_ROOT/gspc-feynman && \
        cd $software_ROOT/gspc-feynman/template && \
        rm -rf build && mkdir build && cd build && \
        cmake .. \
            -DCMAKE_INSTALL_PREFIX=$software_ROOT/gspc-feynman \
            -DSINGULAR_HOME=$SINGULAR_INSTALL_DIR \
            -DFLINT_HOME=$FLINT_HOME \
            -DGMP_HOME=$GMP_HOME \
            -DBOOST_ROOT=$BOOST_ROOT \
            -DBoost_NO_BOOST_CMAKE=ON && \
        make -j$(nproc) && \
        make install
    
    # Setup GPI config files, examples, and /etc/hosts entry early to prevent hostname resolution errors
    RUN mkdir -p $software_ROOT/tempdir && \
        hostname > $software_ROOT/nodefile && \
        hostname > $software_ROOT/loghostfile && \
        echo "127.0.0.1 $(hostname)" >> /etc/hosts && \
        mkdir -p $GSPC_FEYNMAN_INSTALL_DIR/share/examples && \
        cp $software_ROOT/gspc-feynman/template/examples/*.lib $GSPC_FEYNMAN_INSTALL_DIR/share/examples/ && \
        ln -s $GSPC_FEYNMAN_INSTALL_DIR/share/examples/templategspc.lib $GSPC_FEYNMAN_INSTALL_DIR/templategspc.lib
    
    ENV GSPC_FEYNMAN_EXAMPLES_DIR=$GSPC_FEYNMAN_INSTALL_DIR/share/examples
    
    # Setup SSH
    RUN mkdir -p /var/run/sshd /root/.ssh && \
        echo 'root:root' | chpasswd && \
        sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
        sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config && \
        sed -i 's/#PubkeyAuthentication yes/PubkeyAuthentication yes/' /etc/ssh/sshd_config && \
        if [ ! -f /root/.ssh/id_rsa ]; then ssh-keygen -t rsa -b 4096 -N '' -f /root/.ssh/id_rsa; fi && \
        cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys && \
        chmod 600 /root/.ssh/authorized_keys && \
        echo "Host *\n    StrictHostKeyChecking no\n" > /root/.ssh/config && \
        chmod 600 /root/.ssh/config

    # Copy entrypoint script
    COPY entrypoint.sh /entrypoint.sh
    RUN chmod +x /entrypoint.sh

    # Expose monitor port
    EXPOSE 9876
    
    # Set entrypoint and command
    ENTRYPOINT ["/entrypoint.sh"]
    CMD ["bash"]
    