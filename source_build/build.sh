#!/bin/bash

# Load the architecture choice from Kconfig
make config

# Extract selected architecture
source .config

if [[ "$CONFIG_ARCH_ARM" == "y" ]]; then
    ARCH="arm"
    CROSS_COMPILE="arm-linux-androideabi-"
elif [[ "$CONFIG_ARCH_ARM64" == "y" ]]; then
    ARCH="arm64"
    CROSS_COMPILE="aarch64-linux-android-"
elif [[ "$CONFIG_ARCH_X86_64" == "y" ]]; then
    ARCH="x86_64"
    CROSS_COMPILE="x86_64-linux-android-"
else
    echo "No architecture selected. Exiting..."
    exit 1
fi

echo "Building for $ARCH with $CROSS_COMPILE"

# Build the module
make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE -j$(nproc)

# Check for errors
if [[ $? -ne 0 ]]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful! Module created."
