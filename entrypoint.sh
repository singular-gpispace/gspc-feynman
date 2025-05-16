#!/bin/bash

# Create nodefile with the runtime hostname
echo "$(hostname)" > /singular-gpispace/nodefile

# Ensure hostname is resolvable
echo "$(hostname -i) $(hostname)" >> /etc/hosts

# Start SSH server
service ssh start

# Print startup info
echo "Container Hostname: $(hostname)"
echo "Nodefile contents:"
cat /singular-gpispace/nodefile
echo "Hosts file:"
cat /etc/hosts

# Start interactive shell
exec "$@" 