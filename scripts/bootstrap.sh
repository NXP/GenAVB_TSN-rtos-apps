#!/bin/bash -e

if [ "${BASH_SOURCE[0]}" = "${0}" ]; then
    echo "Error: This script must be sourced, not executed directly."
    echo "Usage: source bootstrap.sh"
    echo "   or: . bootstrap.sh"
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(dirname "$(dirname "$SCRIPT_DIR")")"
ARMGCC_VERSION="14.2.rel1"
ARMGCC_NAME="arm-gnu-toolchain-${ARMGCC_VERSION}-x86_64-arm-none-eabi"

echo "Bootstrapping MCUX SDK 3.0 development environment..."

echo "Running initial workspace setup..."
"$SCRIPT_DIR/setup_env.sh"
if [ $? -ne 0 ]; then
    echo "Error: Initial setup failed"
    return 1
fi

echo "Setting up environment variables..."
cd "$WORKSPACE_DIR"
. .venv/bin/activate

FOUND_TOOLCHAIN=0
if [ -n "$ARMGCC_DIR" ]; then
    echo "ARM GCC toolchain already set: $ARMGCC_DIR"
    FOUND_TOOLCHAIN=1
elif [ -d "$WORKSPACE_DIR/tools/$ARMGCC_NAME" ]; then
    export ARMGCC_DIR="$WORKSPACE_DIR/tools/$ARMGCC_NAME"
    FOUND_TOOLCHAIN=1
    echo "Found ARM GCC toolchain: $ARMGCC_DIR"
fi

if [ "$FOUND_TOOLCHAIN" = "0" ]; then
    echo "Error: ARM GCC toolchain not found. Please set ARMGCC_DIR manually"
    echo "or ensure toolchain is installed in tools directory"
    return 1
fi

cd mcuxsdk

echo ""
echo "========================================"
echo "Environment setup completed successfully!"
echo "========================================"
echo "Virtual environment: $WORKSPACE_DIR/.venv"
echo "ARM GCC toolchain: $ARMGCC_DIR"
echo "Current directory: $(pwd)"
echo ""
echo "You are now ready to build. Example:"
echo "  west build -p always examples/demo_apps/avb_tsn/tsn_app --toolchain armgcc --config release -b evkbmimxrt1170 -Dcore_id=cm7"
