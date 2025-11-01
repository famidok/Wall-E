#!/bin/bash

INTERFACES=$(sudo xdp-loader status | awk 'NR > 4 && $0 !~ /<No XDP program loaded!>/ && NF > 0 { print $1 }')

echo -e "Program loaded interfaces list: \n$INTERFACES"
read -p "Enter the interface name to unload XDP program from (e.g., enp0s1): " IFACE

echo "Trying to unload XDP program from interface: $IFACE"
sudo xdp-loader unload -a "$IFACE" || echo "No XDP program was loaded on $IFACE."

echo "Switching to root to remove /sys/fs/bpf/xdp/globals..."
sudo su -c 'rm -rf /sys/fs/bpf/xdp/globals'

echo "Cleanup completed."
