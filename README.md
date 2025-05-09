# 🚀 Parallel Computation of Short IPB Systems

![Petri Net](template/cmake/modules/web.png)

## 📋 Overview

This package provides a framework for automating the reduction of Feynman integrals using Integration-by-Parts (IBP) identities and sector decomposition. It transforms sector structures into directed acyclic graphs (DAGs) and generates Petri net representations for parallel computation using GPI-Space.

## 🛠️ Installation Guide

### Prerequisites

## **Using Spack Package Manager**

#### Required System Packages
```bash

sudo apt update
sudo apt install -y build-essential ca-certificates coreutils curl \
    environment-modules gfortran git gpg lsb-release python3 \
    python3-distutils python3-venv unzip zip
```

### Step 1: Environment Setup

```bash
# Create and set up working directory
mkdir -p ~/singular-gpispace
cd ~/singular-gpispace

# Set environment variables (add to ~/.bashrc for persistence)
echo 'export software_ROOT=~/singular-gpispace' >> ~/.bashrc
echo 'export install_ROOT=~/singular-gpispace' >> ~/.bashrc
source ~/.bashrc
```

### Step 2: Install Spack

```bash
# Clone Spack
git clone https://github.com/spack/spack.git $software_ROOT/spack

# Switch to stable version
cd $software_ROOT/spack
git checkout releases/v0.21

# Initialize Spack
. $software_ROOT/spack/share/spack/setup-env.sh

# Bootstrap Spack
spack spec zlib
```

### Step 3: Install gspc-feynman

```bash
# Add package repository
git clone https://github.com/singular-gpispace/spack-packages.git $software_ROOT/spack-packages
spack repo add $software_ROOT/spack-packages

# Install package
spack install gspc-feynman
```

### Step 4: Configure Environment

```bash
# Set environment variables
export GSPC_FEYNMAN_INSTALL_DIR=$(spack location -i gspc-feynman)
```

## 🚀 Quick Start

### Option 1: Using Setup Script (Recommended)

```bash
# The setup script will:
#  Start the GPI-Space monitor
$GSPC_FEYNMAN_INSTALL_DIR/libexec/setup.sh
```

### Resume After Setup

After pressing Ctrl+C to stop watching the monitor, you can continue in the same terminal:

```bash
# 2. Change to the working directory ($software_ROOT)
cd $software_ROOT
# Start Singular
Singular
```

## 🧪 Example Usage

```singular
LIB "templategspc.lib";

// Configure GPI-Space
configToken gc = configure_gspc();
gc.options.tmpdir = "tempdir";
gc.options.nodefile = "nodefile";
gc.options.procspernode = 4;
gc.options.loghostfile = "loghostfile";
gc.options.logport = 9876;

// Define graph structure
ring R = 0, x, dp;
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
);

// Define target integrals
list targetInt = list(
    list(1, 1, 1, -1, -3, -1, -1, -1, -1),
    list(1, -1, 1, -1, -3, -1, -1, -4, -1)
);

// Compute sector decomposition
list L = v, e, targetInt;
def re = gspc_feynman(L, gc);
```

### Option 2: Manual Setup (Alternative)

If you don't have access to the screen or prefer to use  a file:

```bash
# Create logs directory
mkdir -p $software_ROOT/logs

# Get GPI-Space binary path
GPISPACE_BIN=$(spack location -i gpi-space@24.12)/bin

# Start monitor with logging
[ -f $software_ROOT/logs/monitor.txt ] && rm $software_ROOT/logs/monitor.txt
cd $GPISPACE_BIN && ./gspc-logging-to-stdout.exe --port 9876 >> $software_ROOT/logs/monitor.txt 2>&1 &

# Monitor the output
tail -f $software_ROOT/logs/monitor.txt

# Note: You can press Ctrl+C to stop watching the logs and continue using the same terminal
# To return to monitoring the logs, simply run:
# tail -f $software_ROOT/logs/monitor.txt
```

## 🔧 Troubleshooting

### Common Issues

1. **Monitor Not Starting**
   ```bash
   # Check if port is in use
   netstat -tuln | grep 9876
   
   # Check log file
   cat $software_ROOT/logs/monitor.txt
   ```

2. **Module Loading Issues**
   ```bash
   # Reload modules
   spack load gpi-space@24.12
   spack load singular@4.4.0p2
   spack load gspc-feynman
   ```

3. **SSH Connection Issues**
   ```bash
   # Test SSH connection
   ssh localhost echo "SSH connection successful"
   
   # Regenerate SSH keys if needed
   ssh-keygen -t rsa -b 4096 -N '' -f ~/.ssh/id_rsa
   ssh-copy-id -f -i ~/.ssh/id_rsa localhost
   ```

### Environment Verification

```bash
# Check environment variables
echo "SINGULAR_INSTALL_DIR: $SINGULAR_INSTALL_DIR"
echo "GSPC_FEYNMAN_INSTALL_DIR: $GSPC_FEYNMAN_INSTALL_DIR"
echo "GSPC_FEYNMAN_EXAMPLES_DIR: $GSPC_FEYNMAN_EXAMPLES_DIR"
echo "SINGULARPATH: $SINGULARPATH"

# Check file permissions
ls -l $software_ROOT/tempdir
ls -l $software_ROOT/nodefile
ls -l $software_ROOT/loghostfile
```

## 📚 Additional Resources

- [GPI-Space Documentation](https://github.com/singular-gpispace/gspc-feynman)
- [Singular Documentation](https://www.singular.uni-kl.de/)
- [Spack Documentation](https://spack.readthedocs.io/)

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## 📄 License

This project is licensed under the Apache License 2.0 - see the LICENSE file for details.
