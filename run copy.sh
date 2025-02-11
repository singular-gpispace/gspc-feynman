cd /home/atraore/gpi/try_gpi/feynman_ibp/example_dir
rm -rf temp/*
export GPISPACE_ROOT="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gpi-space-24.12-jz6b4m6ql54fmhkpq6gbico2neic3kd5"


# Load correct GPI-Space version
spack load gpi-space@24.12


export GPISPACE_ROOT="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/gpi-space-23.06-6oiwgcz4ou6sktku324hoesngmqoiv6b/"
spack load gpi-space@23.06

# Set Singular install 4.4.0p2
SINGULAR_INSTALL_DIR="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-4.4.0p2-k7rgdkzo5prqsvxjckejvcdvxgjr64bk"

#old 4.3.0
#SINGULAR_INSTALL_DIR="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-snapshot_22_03-5jvwtprazqirywu2triw6rprjazzi3so/"

export PATH=$SINGULAR_INSTALL_DIR/bin:$PATH

# Set FLINT home directory
FLINT_HOME="/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/flint-2.6.3-pb3i4qjyjz7pqkpf6cs7wk6ro5pl564i"
export LD_LIBRARY_PATH=$FLINT_HOME/lib:$LD_LIBRARY_PATH

# Run Singular with example script
SINGULARPATH=/home/atraore/gpi/try_gpi/feynman_ibp/install_dir Singular example.sing
