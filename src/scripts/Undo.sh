#!/bin/bash

read -p "Enter the interface name to unload XDP program from (e.g., enp0s1): " IFACE

echo "Trying to unload XDP program from interface: $IFACE"
sudo xdp-loader unload -a "$IFACE" || echo "No XDP program was loaded on $IFACE."

echo "Switching to root to remove /sys/fs/bpf/xdp/globals..."
sudo su -c 'rm -rf /sys/fs/bpf/xdp/globals'

echo "Cleanup completed."
