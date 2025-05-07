# Feynman Module Examples

This directory contains example files demonstrating the usage of the Feynman module for computing Feynman integrals using GPI-Space.

## Available Examples

### feynman_example.lib
This example demonstrates:
- Setting up GPI-Space configuration
- Defining a Feynman graph with vertices and edges
- Computing Feynman integrals using parallel computation
- Timing the computation

## How to Use

1. Make sure GPI-Space is running and configured:
```bash
# Start GPI-Space logging
gspc-logging-to-stdout.exe --port 9876
```

2. Start Singular:
```bash
Singular
```

3. Load and run the example:
```
LIB "feynman_example.lib";
```

4. The computation will run and display the results along with timing information.

## Requirements
- Singular 4.4.0p2 or later
- GPI-Space 24.12 or later
- FLINT 2.6.3 or later

## Configuration
The example uses the following GPI-Space configuration:
- 7 processes per node
- Port 9876 for logging
- Temporary directory: /tmp/feynman_example

You can modify these settings in the example file to match your environment. 