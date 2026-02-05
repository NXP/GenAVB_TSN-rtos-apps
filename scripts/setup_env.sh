#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_DIR="$(dirname "$SCRIPT_DIR")"
WORKSPACE_DIR="$(dirname "$REPO_DIR")"
TOOLS_DIR="$WORKSPACE_DIR/tools"
ARMGCC_VERSION="14.2.rel1"
ARMGCC_NAME="arm-gnu-toolchain-${ARMGCC_VERSION}-x86_64-arm-none-eabi"
ARMGCC_URL="https://developer.arm.com/-/media/Files/downloads/gnu/${ARMGCC_VERSION}/binrel/arm-gnu-toolchain-${ARMGCC_VERSION}-x86_64-arm-none-eabi.tar.xz"

echo "Setting up MCUX SDK 3.0 workspace environment..."

cd "$WORKSPACE_DIR"
if [ $? -ne 0 ]; then
    echo "Error: Could not change to workspace directory"
    exit 1
fi

SKIP_WEST=0
if [ -d "$WORKSPACE_DIR/mcuxsdk-manifests" ]; then
    echo "West environment already initialized - skipping west setup"
    SKIP_WEST=1
fi

if [ "$SKIP_WEST" = "0" ]; then
    echo "Creating Python virtual environment..."
    if [ -d ".venv" ]; then
        echo "Removing existing virtual environment..."
        rm -rf .venv
    fi

    python3 -m venv .venv
    if [ $? -ne 0 ]; then
        echo "Error: Failed to create virtual environment"
        exit 1
    fi

    . .venv/bin/activate
    if [ $? -ne 0 ]; then
        echo "Error: Failed to activate virtual environment"
        exit 1
    fi

    echo "Installing Python requirements..."
    pip install -r "$SCRIPT_DIR/env_requirements.txt"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to install Python requirements"
        exit 1
    fi

    export PATH="$WORKSPACE_DIR/.venv/bin:$PATH"


    west update

    echo "Installing MCUX SDK requirements..."
    cd mcuxsdk
    pip install -r scripts/requirements.txt
    if [ $? -ne 0 ]; then
        echo "Error: Failed to install MCUX SDK requirements"
        exit 1
    fi
    cd ..

    if [ -d "$REPO_DIR/patches" ]; then
        echo "Applying patches..."

        west patch -b "$REPO_DIR/patches" -l "$REPO_DIR/patches/patches.yml" apply
        if [ $? -ne 0 ]; then
            echo "Error: west patch failed"
            exit 1
        fi
    fi
else
    echo "Activating existing virtual environment..."
    if [ ! -d ".venv" ]; then
        echo "Creating Python virtual environment..."
        python3 -m venv .venv
        if [ $? -ne 0 ]; then
            echo "Error: Failed to create virtual environment"
            exit 1
        fi
    fi

    . .venv/bin/activate
    if [ $? -ne 0 ]; then
        echo "Error: Failed to activate virtual environment"
        exit 1
    fi
fi

SKIP_TOOLCHAIN=0
if [ -n "$ARMGCC_DIR" ]; then
    echo "ARM GCC toolchain path already set: $ARMGCC_DIR"
    SKIP_TOOLCHAIN=1
elif [ -d "$TOOLS_DIR/$ARMGCC_NAME" ]; then
    echo "Found existing ARM GCC toolchain: $TOOLS_DIR/$ARMGCC_NAME"
    SKIP_TOOLCHAIN=1
fi

if [ "$SKIP_TOOLCHAIN" = "0" ]; then
    echo "Downloading ARM GCC toolchain..."
    mkdir -p "$TOOLS_DIR"
    curl -L -o "$TOOLS_DIR/arm-gcc.tar.xz" "$ARMGCC_URL"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to download ARM GCC toolchain"
        exit 1
    fi

    echo "Extracting ARM GCC toolchain..."
    cd "$TOOLS_DIR"
    tar -xf arm-gcc.tar.xz
    if [ $? -ne 0 ]; then
        echo "Error: Failed to extract ARM GCC toolchain"
        exit 1
    fi

    rm arm-gcc.tar.xz
    cd "$WORKSPACE_DIR"
else
    echo "Skipping ARM GCC toolchain setup"
fi

echo "Creating symbolic links..."

if [ -L "$WORKSPACE_DIR/mcuxsdk/examples/demo_apps/avb_tsn" ]; then
    rm -f "$WORKSPACE_DIR/mcuxsdk/examples/demo_apps/avb_tsn"
fi
ln -snf "$REPO_DIR/boards/src/demo_apps/avb_tsn" "$WORKSPACE_DIR/mcuxsdk/examples/demo_apps/avb_tsn"

for board_dir in "$REPO_DIR/boards"/*; do
    if [ -d "$board_dir" ]; then
        BOARD_NAME=$(basename "$board_dir")
        if [ "$BOARD_NAME" != "src" ]; then
            echo "Creating symlink for board: $BOARD_NAME"
            if [ -L "$WORKSPACE_DIR/mcuxsdk/examples/_boards/$BOARD_NAME/demo_apps/avb_tsn" ]; then
                rm -f "$WORKSPACE_DIR/mcuxsdk/examples/_boards/$BOARD_NAME/demo_apps/avb_tsn"
            fi
            ln -sf "$REPO_DIR/boards/$BOARD_NAME/demo_apps/avb_tsn" "$WORKSPACE_DIR/mcuxsdk/examples/_boards/$BOARD_NAME/demo_apps/avb_tsn"
        fi
    fi
done

if [ -L "$WORKSPACE_DIR/mcuxsdk/devices/RT/RT1180/MIMXRT118x" ]; then
    rm -f "$WORKSPACE_DIR/mcuxsdk/devices/RT/RT1180/MIMXRT118x"
fi
ln -snf "$REPO_DIR/devices/MIMXRT118x" "$WORKSPACE_DIR/mcuxsdk/devices/RT/RT1180/MIMXRT118x"
