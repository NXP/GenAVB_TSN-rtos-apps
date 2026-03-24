# GenAVB/TSN RTOS Applications

GenAVB/TSN stack reference applications for RTOS covering both AVB and TSN use cases, running on i.MX RTxxx and MCX E series family of devices.

## Overview

This repository provides a comprehensive set of AVB/TSN example applications demonstrating:
* Audio Video Bridging (AVB) capabilities
* Time-Sensitive Networking (TSN) features

## Supported Hardware

This release supports the following i.MX RTxxx EVK boards:

| SoC            | Board Name        | Description                    |
|----------------|-------------------|--------------------------------|
| i.MX RT1150    | evkbimxrt1050     | i.MX RT1050 Evaluation Kit     |
| i.MX RT1170    | evkbmimxrt1170    | i.MX RT1170 Evaluation Kit     |
| i.MX RT1180    | evkmimxrt1180     | i.MX RT1180 Evaluation Kit     |
| i.MX RT1186    | frdmimxrt1186     | i.MX RT1186 Freedom Board      |
| MCX E24        | frdmmcxe247       | MCX E247 Freedom Board         |
| MCX E31        | frdmmcxe31b       | MCX E31B Freedom Board         |

## Dependencies

The example applications have dependencies on:
* **MCUXpresso SDK** - Provides peripheral drivers and components support
* **GenAVB/TSN Stack** - Core AVB/TSN protocol stack
* **RTOS Abstraction Layer** - OS abstraction for portability
* **RTOS Application Layer** - Common application framework
* **FreeRTOS Kernel** - Real-time operating system

All dependencies are managed through the [Zephyr west tool](https://docs.zephyrproject.org/latest/guides/west/index.html), which helps maintain multiple repositories.

## Getting Started

### Prerequisites

#### ARM GNU Toolchain

Download and install ARM GCC toolchain version 14.2.Rel1:

**Linux:**
```bash
wget https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz
sudo tar -C /opt/ -xf arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz
export ARMGCC_DIR=/opt/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi
```

**Windows:**

Download the installer from [ARM Developer](https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-mingw-w64-i686-arm-none-eabi.exe) and install it, then set the environment variable:
```cmd
set ARMGCC_DIR=C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.2 Rel1
```

#### Host Tools

**Linux (Ubuntu/Debian):**
```bash
sudo apt update
sudo apt install git python3 python3-pip cmake ninja-build
pip3 install west # you might need a wirtual environment here
```

**Windows:**
- Install [Git for Windows](https://git-scm.com/download/win)
- Install [Python 3.10+](https://www.python.org/downloads/windows/)
- Install [CMake 3.30.0+](https://cmake.org/download/) - use the .msi installer
- Install west:
```cmd
pip install west
```

## Cloning the Repository

Initialize a west workspace to start the development environment:

**Linux:**
```bash
export tag=<release-tag>
west init -m https://github.com/NXP/GenAVB_TSN-rtos-apps --mr ${tag} <workspace>
cd <workspace>/GenAVB_TSN-rtos-apps
```

**Windows:**
```cmd
set tag=<release-tag>
west init -m https://github.com/NXP/GenAVB_TSN-rtos-apps.git <workspace> --mr %tag%
cd <workspace>\GenAVB_TSN-rtos-apps
```

## Repository Structure

```
GenAVB_TSN-rtos-apps/
├── boards/
│   ├── src/                          # Board-agnostic application source code
│   │   └── demo_apps/
│   │       └── avb_tsn/
│   │           ├── avb_audio_app/    # AVB audio streaming application
│   │           ├── audio_app/        # Audio application
│   │           ├── tsn_app/          # TSN networking application
│   │           ├── dsa_enetc/        # DSA ENETC application
│   │           ├── dsa_switch/       # DSA switch application
│   │           └── common/           # Application common code
│   ├── evkbimxrt1050/               # Board-specific files for RT1050
│   ├── evkbmimxrt1170/              # Board-specific files for RT1170
│   ├── evkmimxrt1180/               # Board-specific files for RT1189
│   ├── frdmimxrt1186/               # Board-specific files for RT1186
│   ├── frdmmcxe247/                 # Board-specific files for MCX E247
│   └── frdmmcxe31b/                 # Board-specific files for MCX E31B
├── devices/                          # SoC-specific shared files
├── scripts/                          # Setup scripts
│   ├── bootstrap.sh                 # Linux automated setup
│   ├── bootstrap.bat                # Windows automated setup
│   ├── setup_env.sh                 # Linux environment setup
│   ├── setup_env.bat                # Windows environment setup
│   └── requirements.txt             # Python dependencies
├── west.yml                         # West manifest for dependencies
└── README.md                        # This file
```

## Build Instructions

### Automated Setup

The bootstrap scripts automate the entire setup process including workspace initialization, toolchain download, symbolic links creation, and Python environment setup.

**Linux:**
```bash
cd <workspace>/GenAVB_TSN-rtos-apps/scripts
source bootstrap.sh
```

**Windows (Run Command Prompt as Administrator):**
```cmd
cd <workspace>\GenAVB_TSN-rtos-apps\scripts
bootstrap.bat
```

The bootstrap scripts will:
- Update the west workspace with all required repositories
- Download and extract ARM GCC toolchain (if `ARMGCC_DIR` not set)
- Create required symbolic links
- Set up Python virtual environment
- Install all required Python packages

After bootstrap completes, the repo is ready to build applications.

### Netconf Support prerequisites
GenAVB/TSN release provides Netconf support for i.MX RT1180 board. To enable Netconf support, ensure the following prerequisites are met:

Download Netconf additionals packages (sysrepo-genavb , mcux-yang and mcux-netconf) from nxp.com or contact NXP support.

Install sysrepo-genavb sources in the west workspace:

```cmd
cd <workspace>/mcuxsdk/components
tar -xzf  sysrepo-genavb-<version>.tar.gz
```

Install mcux-yang sources in the workspace:

```cmd
cd <workspace>/mcuxsdk/components
tar -xzf mcux-yang-<version>.tar.gz
```

Install mcux-netconf sources in the workspace:

```cmd
cd <workspace>
unzip mcux-netconf-<version>.zip
```

### Building Applications

Navigate to the SDK root directory and use the `west build` command:

```bash
cd <workspace>/mcuxsdk
west build -p always examples/demo_apps/avb_tsn/tsn_app --toolchain armgcc --config release -b evkbmimxrt1170 -Dcore_id=cm7
```

Build artifacts will be in the `build/` directory.

**Build Command Parameters:**
- `-p always` - Pristine build (clean before building)
- `--toolchain armgcc` - Use ARM GCC toolchain
- `--config release` - Build in release configuration (use `debug` for debug build)
- `-b <board>` - Target board name (e.g., evkbmimxrt1170, evkmimxrt1180)
- `-Dcore_id=<core>` - Target core (cm7, cm33, etc.)
- `-d <dir>` - Build directory (optional, defaults to `build/`)

## Activating the Virtual Environment

After the initial setup, when opening a new terminal session, you need to activate the virtual environment:

**Linux:**
```bash
cd <workspace>
source .venv/bin/activate
export ARMGCC_DIR=/opt/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi
```

**Windows:**
```cmd
cd <workspace>
.venv\Scripts\activate.bat
set ARMGCC_DIR=C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.2 Rel1
```
