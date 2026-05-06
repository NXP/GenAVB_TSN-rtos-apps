#!/bin/bash -e

if [ "${BASH_SOURCE[0]}" = "${0}" ]; then
    echo "Error: This script must be sourced, not executed directly."
    echo "Usage: source bootstrap.sh"
    echo "   or: . bootstrap.sh"
    return 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(dirname "$(dirname "$SCRIPT_DIR")")"
REPO_DIR="$(dirname "$SCRIPT_DIR")"
ARMGCC_VERSION="14.2.rel1"
ARMGCC_NAME="arm-gnu-toolchain-${ARMGCC_VERSION}-x86_64-arm-none-eabi"

echo "Bootstrapping MCUX SDK development environment..."

cd "$WORKSPACE_DIR"
if [ $? -ne 0 ]; then
    echo "Error: Could not change to workspace directory"
    return 1
fi

SKIP_WEST=0
if [ -f "$WORKSPACE_DIR/.env_created" ]; then
    echo "West environment already initialized - skipping west setup"
    SKIP_WEST=1
fi

if [ "$SKIP_WEST" = "0" ]; then
    # BEGIN_INTERNAL
    # Run internal setup
    if [ -f "$SCRIPT_DIR/internal.sh" ]; then
        source "$SCRIPT_DIR/internal.sh"
        if [ $? -ne 0 ]; then
            echo "Error: Internal setup failed"
            return 1
        fi
    fi
    # END_INTERNAL

    west update
    if [ $? -ne 0 ]; then
        echo "Error: Failed to update west workspace"
        return 1
    fi

    if [ -d "$REPO_DIR/patches" ]; then
        echo "Applying patches..."

        west patch -b "$REPO_DIR/patches" -l "$REPO_DIR/patches/patches.yml" apply
        if [ $? -ne 0 ]; then
            echo "Error: west patch failed"
            return 1
        fi
    fi

    touch "$WORKSPACE_DIR/.env_created"
fi

echo "Setting up environment variables..."
cd "$WORKSPACE_DIR"

FOUND_TOOLCHAIN=0
if [ -n "$ARMGCC_DIR" ]; then
    echo "ARM GCC toolchain already set: $ARMGCC_DIR"
    FOUND_TOOLCHAIN=1
elif [ -d "$HOME/.mcuxpressotools/$ARMGCC_NAME" ]; then
    export ARMGCC_DIR="$HOME/.mcuxpressotools/$ARMGCC_NAME"
    FOUND_TOOLCHAIN=1
    echo "Found ARM GCC toolchain: $ARMGCC_DIR"
fi

if [ "$FOUND_TOOLCHAIN" = "0" ]; then
    echo "Error: ARM GCC toolchain not found. Please set ARMGCC_DIR manually"
    return 1
fi

cd $REPO_DIR

echo ""
echo "========================================"
echo "Environment setup completed successfully!"
echo "========================================"
echo "Virtual environment: $VIRTUAL_ENV"
echo "ARM GCC toolchain: $ARMGCC_DIR"
echo "Current directory: $(pwd)"
echo ""
echo "You are now ready to build. Example:"
echo "  west build -p always boards/src/demo_apps/avb_tsn/tsn_app --toolchain armgcc --config release -b evkbmimxrt1170 -Dcore_id=cm7"
