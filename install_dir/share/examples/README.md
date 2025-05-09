# GSPC-Feynman Examples

This directory contains example files demonstrating the usage of the GSPC-Feynman module.

## Requirements

- Singular 4.4.0p2 or later
- GPI-Space 24.12 or later
- FLINT 2.6.3 or later
- Boost 1.63.0 or later (with serialization, system, filesystem, program_options, thread, chrono, date_time, and atomic components)

## Setup

1. Load the gspc-feynman module:
   ```bash
   spack load gspc-feynman
   ```

2. Create necessary configuration files:
   ```bash
   hostname > nodefile
   mkdir -p temp
   hostname > loghostfile
   ```

3. Start the GPI-Space monitor:
   ```bash
   gspc-monitor --port 9876 &
   ```

## Example Files

- `feynman_example.lib`: Contains example code for using the Feynman module with GPI-Space configuration and graph computations.
- `templategspc.lib`: The main library file containing the GPI-Space interface procedures.
- `templategp.lib`: Supporting library file for graph computations.

## Usage

1. Make sure all dependencies are installed and properly configured.
2. The examples directory is automatically installed to `${CMAKE_INSTALL_PREFIX}/share/examples` during package installation.
3. You can access the examples directory through the `GSPC_FEYNMAN_EXAMPLES_DIR` environment variable.

## Configuration

The example file includes GPI-Space configuration settings that you may need to adjust for your environment:
- `tmpdir`: Directory for temporary files
- `nodefile`: File containing node information
- `procspernode`: Number of processes per node
- `loghost`: Host for logging
- `logport`: Port for logging

## Running Examples

To run the examples in Singular:

```bash
SINGULARPATH="$GSPC_FEYNMAN_INSTALL_DIR" Singular
> LIB "templategspc.lib";
> < "feynman_example.lib";
```

Note: Make sure the `SINGULARPATH` environment variable is set correctly to find the library files. 