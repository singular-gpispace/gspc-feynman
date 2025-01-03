cd /home/atraore/gpi/try_gpi/feynman_ibp/example_dir
rm -rf temp/*
spack load gpi-space@23.06

export PATH=/home/atraore/Singular4/bin:$PATH
FLINT_HOME="/usr/local/"

# Set FLINT home directory
FLINT_HOME="/usr/local/"

# Set the library path (corrected syntax)
export LD_LIBRARY_PATH=/home/atraore/git/Singular4/flint/flint2:$LD_LIBRARY_PATH

# Run Singular with the example script
SINGULARPATH=/home/atraore/gpi/try_gpi/feynman_ibp/install_dir Singular example.sing
