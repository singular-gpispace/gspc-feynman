# **Parallel Computation of Short IPB Systems: Automated Translation of the Web of Sectors into a Petri Net**

<div align="center">
  <img src="template/cmame/modules/web.png" alt="Petri Net" width="300"/>
</div>

## **Introduction**

This package follows the structure of the modular package available at https://github.com/singular-gpispace/modular_res.

This package provides a framework for automating the reduction of Feynman integrals using Integration-by-Parts (IBP) identities and sector decomposition. We implement a novel approach that transforms the sector structure of a given Feynman integral into a directed acyclic graph (DAG) and subsequently generates a Petri net representation. The Petri net is then encoded in XPNet format, allowing its execution within the \textsc{GPI-Space} parallel workflow system. This enables efficient large-scale IBP reduction computations by distributing tasks across multiple computing nodes. Our implementation is built upon the \texttt{feynman.lib} \cite{feynman_ibp} library in Singular and provides a scalable solution to IBP reductions in multi-loop Feynman integrals.

# **Installation Guide**

## **Using Spack Package Manager**

Spack is a package manager specifically designed for handling software installations in supercomputing environments, but usable on anything from a personal computer to an HPC cluster. It supports Linux and macOS (note that the Singular/GPI-Space framework and hence our package requires Linux).

### **Environment Setup**

1. Set up environment variables:
   
   ```bash
   export software_ROOT=~/singular-gpispace
   export install_ROOT=~/singular-gpispace
   ```
   
   Note: These variables need to be set again if you open a new terminal session. It's recommended to add these lines to your `.profile` file.

### **Installing Spack**

1. Clone Spack from Github:
   
   ```bash
   git clone https://github.com/spack/spack.git $software_ROOT/spack
   ```
2. Check out version v0.21:
   
   ```bash
   cd $software_ROOT/spack
   git checkout releases/v0.21
   cd $software_ROOT
   ```
3. Install required system packages (Ubuntu):
   
   ```bash
   sudo apt update
   sudo apt install build-essential ca-certificates coreutils curl environment-modules gfortran git gpg lsb-release python3 python3-distutils python3-venv unzip zip
   ```
4. Set up Spack environment:
   
   ```bash
   . $software_ROOT/spack/share/spack/setup-env.sh
   ```
5. Bootstrap clingo:
   
   ```bash
   spack spec zlib
   ```
   
   Note: If experiencing connection timeouts, adjust the `connect_timeout` value in:
   
   ```bash
   vim $software_ROOT/spack/etc/spack/defaults/config.yaml
   ```

### **Installing gspc-feynman**

1. Clone the Singular/GPI-Space package repository:
   
   ```bash
   git clone https://github.com/singular-gpispace/spack-packages.git $software_ROOT/spack-packages
   ```
2. Add the repository to Spack:
   
   ```bash
   spack repo add $software_ROOT/spack-packages
   ```
3. Install gspc-feynman:
   
   ```bash
   spack install gspc-feynman
   ```

### **Optional: Environment-based Installation**

```bash
spack env create myenv
spack env activate -p myenv
spack add gspc-feynman
spack concretize
spack install
```

## **Usage Guide**

### **Loading the Package**

```bash
spack load gspc-feynman
```

For environment-based installation:

```bash
spack env activate -p myenv
```

To deactivate the environment:

```bash
spack env deactivate
```

### **SSH Configuration**

1. Generate SSH key:
   
   ```bash
   ssh-keygen -t rsa -b 4096 -N '' -f "${HOME}/.ssh/id_rsa"
   ```
2. Install SSH server (Ubuntu):
   
   ```bash
   sudo apt install openssh-server
   ```
3. Register your key:
   
   ```bash
   ssh-copy-id -f -i "${HOME}/.ssh/id_rsa" "${HOSTNAME}"
   ```

### **Running Computations**

1. Set up required directories:
   
   ```bash
   cp $MODULAR_INSTALL_DIR/share/examples/modulargp.lib $software_ROOT
   hostname > $software_ROOT/nodefile
   mkdir -p $software_ROOT/tempdir
   ```
2. Start GPI-Space Monitor (optional):
   
   ```bash
   hostname > $software_ROOT/loghostfile
   gspc-monitor --port 9876 &
   ```
3. Start Singular:
   
   ```bash
   cd $software_ROOT
   SINGULARPATH="$MODULAR_INSTALL_DIR" Singular
   ```

## **Example: Computing Feynman Integrals**

This example demonstrates how to compute Feynman integrals using our framework. We'll set up a graph structure representing the Feynman diagram and compute its sector decomposition.

```singular
LIB "feynmangspc.lib";

configToken gc = configure_gspc();
gc.options.tmpdir = "tempdir";
gc.options.nodefile = "nodefile";
gc.options.procspernode = 4;
gc.options.loghostfile = "loghostfile";
gc.options.logport = 9876;

ring R = 0, x, dp;

// Define the graph structure
list v = list(1, 2, 3, 4, 5, 6);  // vertices
list e = list(
    list(6, 1),    // edge 6->1
    list(4, 6),    // edge 4->6
    list(1, 2),    // edge 1->2
    list(3, 5),    // edge 3->5
    list(4, 3),    // edge 4->3
    list(2, 5),    // edge 2->5
    list(5, 6),    // edge 5->6
    list(1),       // external leg 1
    list(2),       // external leg 2
    list(3),       // external leg 3
    list(4)        // external leg 4
);  // edges and external legs

// Define target integrals
list targetInt = list(
    list(1, 1, 1, -1, -3, -1, -1, -1, -1),
    list(1, -1, 1, -1, -3, -1, -1, -4, -1)
);

// Combine all data into a single list
list L = v, e, targetInt;

// Compute the sector decomposition
def re = gspc_template_SchRes(L, gc);
re;
```
