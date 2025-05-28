#!/bin/bash

read -p "Which application do you want to run? (blacklist/ratelimit/both): " APP
read -p "Enter the interface name (e.g., enp0s1): " IFACE

# Run the Makefile first
echo "Running make from ../../Makefile..."
make -C ../../
if [ $? -ne 0 ]; then
    echo "Make failed!"
    exit 1
fi

# Paths for blacklist
BLACKLIST_O="../../build/blacklist.o"
BLACKLIST_CONFIG_WRITER="../../build/blacklist_config_writer"
BLACKLIST_MAP="../../build/blacklist_map"

# Paths for ratelimit
RATELIMIT_O="../../build/ratelimit.o"
RATELIMIT_CONFIG_WRITER="../../build/ratelimit_config_writer"
RATELIMIT_MAP="../../build/ratelimit_map"

run_blacklist() {
    echo "Setting up Blacklist..."

    if [ ! -f "$BLACKLIST_O" ]; then
        echo "Error: $BLACKLIST_O not found!"
        exit 1
    fi

    if [ ! -f "$BLACKLIST_CONFIG_WRITER" ]; then
        echo "Error: $BLACKLIST_CONFIG_WRITER not found!"
        exit 1
    fi

    if [ ! -f "$BLACKLIST_MAP" ]; then
        echo "Error: $BLACKLIST_MAP not found!"
        exit 1
    fi

    sudo ip link set "$IFACE" xdpgeneric obj "$BLACKLIST_O" sec prog
    if [ $? -ne 0 ]; then
        echo "Failed to load blacklist.o!"
        exit 1
    fi

    "$BLACKLIST_CONFIG_WRITER"
    if [ $? -ne 0 ]; then
        echo "blacklist_config_writer failed!"
        exit 1
    fi

    sudo "$BLACKLIST_MAP"
    if [ $? -ne 0 ]; then
        echo "blacklist_map failed!"
        exit 1
    fi

    echo "Blacklist setup completed."
}

run_ratelimit() {
    echo "Setting up Ratelimit..."

    if [ ! -f "$RATELIMIT_O" ]; then
        echo "Error: $RATELIMIT_O not found!"
        exit 1
    fi

    if [ ! -f "$RATELIMIT_CONFIG_WRITER" ]; then
        echo "Error: $RATELIMIT_CONFIG_WRITER not found!"
        exit 1
    fi

    if [ ! -f "$RATELIMIT_MAP" ]; then
        echo "Error: $RATELIMIT_MAP not found!"
        exit 1
    fi

    sudo ip link set "$IFACE" xdpgeneric obj "$RATELIMIT_O" sec prog
    if [ $? -ne 0 ]; then
        echo "Failed to load ratelimit.o!"
        exit 1
    fi

    "$RATELIMIT_CONFIG_WRITER"
    if [ $? -ne 0 ]; then
        echo "ratelimit_config_writer failed!"
        exit 1
    fi

    sudo "$RATELIMIT_MAP"
    if [ $? -ne 0 ]; then
        echo "ratelimit_map failed!"
        exit 1
    fi

    echo "Ratelimit setup completed."
}

case "$APP" in
    blacklist)
        run_blacklist
        ;;
    ratelimit)
        run_ratelimit
        ;;
    both)
        run_blacklist
        run_ratelimit
        ;;
    *)
        echo "Invalid option. Please choose 'blacklist', 'ratelimit', or 'both'."
        exit 1
        ;;
esac

echo "All selected operations completed successfully."
