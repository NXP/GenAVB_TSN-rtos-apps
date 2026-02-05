# GenAVB/TSN RTOS Applications

GenAVB/TSN stack reference applications for RTOS covering both AVB and TSN use cases, running on i.MX RT 4-digit and MCX E series family of devices.

## Overview

This repository provides a comprehensive set of AVB/TSN example applications demonstrating:
* Audio Video Bridging (AVB) capabilities
* Time-Sensitive Networking (TSN) features

## Supported Hardware

This release supports the following boards:

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

#### GNU ARM Embedded Toolchain

Download and install GNU ARM toolchain version 14.2.Rel1:

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
pip3 install west # you might need a virtual environment here
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
- Create Python virtual environment (.venv)
- Install all required Python packages (west, MCUX SDK dependencies)
- Update the west workspace with all required repositories
- Apply required patches to MCUX SDK for full application functionality
- Download and extract ARM GCC toolchain (if `ARMGCC_DIR` not set)
- Create required symbolic links
- Export ARMGCC_DIR environment variable
- Navigate to the mcuxsdk directory

After bootstrap completes, the working directory will be mcuxsdk and the environment is ready to build applications.

### Building Applications

Navigate to the SDK root directory and use the `west build` command. The generic command format is:

```bash
west build -p always examples/demo_apps/avb_tsn/<app> --toolchain armgcc --config <config> -b <board> -Dcore_id=<core> -DCONF_FILE=<file>
```
Build artifacts will be generated in the `build/` directory by default.

**Build Command Parameters:**
- `-p always` - Pristine build (clean before building)
- `--toolchain armgcc` - Use ARM GNU toolchain
- `--config <config>` - Build configuration
- `-b <board>` - Target board name (e.g. evkbmimxrt1170, evkmimxrt1180, ...)
- `-Dcore_id=<core>` - Target core (cm7, cm33, ...)
- `-d <dir>` - Build directory (optional, defaults to `build/`)
- `-DCONF_FILE=<file>` - Optional build configuration file

**Build Modes and Configurations**

The detailed combinations of build command parameters are listed below, for each of the supported devices.

**i.MX RT1150**

For i.MX RT1050 two example applications are provided: AVB Endpoint and Milan AVB Endpoint. The applications can be built in “release” mode. 

| **app**            | **config** | **board**    | **core**  | **file** |
|--------------------|-----------------------|-------------------|------------------------|---------------------|
| audio_app          | release               | evkbimxrt1050     |                        |                     |
| avb_audio_app      | release               | evkbimxrt1050     |                        |                     |


**i.MX RT1170**

For i.MX RT1170 three example applications are provided: TSN Endpoint, AVB Endpoint, and Milan AVB Endpoint. Each application can be built in “release” or “ram_release” mode. The “release” images boot from external QuadSPI NOR flash and then relocate to internal memory (Code TCM, System TCM and OCRAM). The “ram_release” images boot from internal memory.

For TSN Endpoint application, the “sdram_release” mode is also available, and it boots from external QuadSPI NOR flash and then relocates to internal memory (Code TCM, System TCM, OCRAM and Sdram).


| **app**            | **config**             | **board**          | **core**                 | **file**  |
|--------------------|------------------------|--------------------|--------------------------|--------------------------|
| tsn_app            | release                | evkbmimxrt1170     | cm7                      |                          |
| tsn_app            | ram_release            | evkbmimxrt1170     | cm7                      |                          |
| tsn_app            | sdram_release          | evkbmimxrt1170     | cm7                      |                          |
| tsn_app            | release_motor          | evkbmimxrt1170     | cm7                      | examples/_boards/evkbmimxrt1170/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf;examples/_boards/evkbmimxrt1170/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf |
| tsn_app            | ram_release_motor      | evkbmimxrt1170     | cm7                      | examples/_boards/evkbmimxrt1170/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf;examples/_boards/evkbmimxrt1170/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf |
| audio_app          | release                | evkbmimxrt1170     | cm7                      |                          |
| audio_app          | ram_release            | evkbmimxrt1170     | cm7                      |                          |
| avb_audio_app      | release                | evkbmimxrt1170     | cm7                      |                          |
| avb_audio_app      | ram_release            | evkbmimxrt1170     | cm7                      |                          |

**i.MX RT1180**

For i.MX RT1180 TSN applications, two example applications with three supported configurations are provided: TSN Bridge, TSN Endpoint and a combined TSN Bridge + Endpoint, which run respectively on Cortex-M33, Cortex-M7 and Cortex-M33 cores. Each application can be built in “release”, “hyperram_release” or “ram_release” mode. The “release” images boot from external QuadSPI NOR flash and then relocate to internal memory (Code TCM, System TCM and OCRAM). The hyperram_release boots from external QuadSPI NOR flash and then relocates to internal memory (Code TCM, System TCM, OCRAM and Hyperram). The “ram_release” images boot from internal memory.

For the TSN Bridge application, two configurations are available (combined with the build mode).

For the TSN Endpoint application, four configurations are available (combined with the build mode).

For i.MX RT1180 DSA applications, two example applications are provided: DSA ENETC CPU port and DSA Switch CPU port, both of which running on Cortex-M33 core. Each application can be built in “release” or “ram_release” mode, following the same boot sequence as the previous section’s description.

| **app**     | **config**                 | **board**          | **core** | **file**  |
|-------------|----------------------------|--------------------|----------|-----------|
| tsn_app     | release                    | evkmimxrt1180      | cm33     |           |
| tsn_app     | hyperram_release           | evkmimxrt1180      | cm33     |           |
| tsn_app     | ram_release                | evkmimxrt1180      | cm33     |           |
| tsn_app     | release_no_enetc0          | evkmimxrt1180      | cm33     | examples/_boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/core/prj_no_enetc0.conf |
| tsn_app     | ram_release_no_enetc0      | evkmimxrt1180      | cm33     | examples/_boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/core/prj_no_enetc0.conf |
| tsn_app     | release_hybrid             | evkmimxrt1180      | cm33     | examples/_boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/core/prj_hybrid.conf |
| tsn_app     | ram_release_hybrid         | evkmimxrt1180      | cm33     | examples/_boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/core/prj_hybrid.conf |
| tsn_app     | release                    | evkmimxrt1180      | cm7      |           |
| tsn_app     | ram_release                | evkmimxrt1180      | cm7      |           |
| tsn_app     | release_enetc0             | evkmimxrt1180      | cm7      | examples/_boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_enetc0.conf |
| tsn_app     | ram_release_enetc0         | evkmimxrt1180      | cm7      | examples/_boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_enetc0.conf |
| tsn_app     | release_motor_controller   | evkmimxrt1180      | cm7      | examples/_boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf |
| tsn_app     | ram_release_motor_controller | evkmimxrt1180    | cm7      | examples/_boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf |
| tsn_app     | release_motor_iodevice     | evkmimxrt1180      | cm7      | examples/_boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf |
| tsn_app     | ram_release_motor_iodevice | evkmimxrt1180      | cm7      | examples/_boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf |
| dsa_enetc     | release                    | evkmimxrt1180      | cm33     |           |
| dsa_enetc     | ram_release                | evkmimxrt1180      | cm33     |           |
| dsa_switch     | release                    | evkmimxrt1180      | cm33     |           |
| dsa_switch     | ram_release                | evkmimxrt1180      | cm33     |           |

**i.MX RT1186**

For i.MX RT1186 TSN applications, two example applications with three supported configurations are provided: TSN Bridge, TSN Endpoint and a combined TSN Bridge + Endpoint, which run respectively on Cortex-M33, Cortex-M7 and Cortex-M33 cores. Each application can be built in “release”, “hyperram_release” or “ram_release” mode. The “release” images boot from external QuadSPI NOR flash and then relocate to internal memory (Code TCM, System TCM and OCRAM). The hyperram_release boots from external QuadSPI NOR flash and then relocates to internal memory (Code TCM, System TCM, OCRAM and Hyperram). The “ram_release” images boot from internal memory.

For the TSN Bridge application, two configurations are available (combined with the build mode).

For the TSN Endpoint application, two configurations are available (combined with the build mode).

| **app**     | **config**                 | **board**          | **core** | **file**  |
|-------------|----------------------------|--------------------|----------|-----------|
| tsn_app     | release                    | frdmimxrt1186      | cm33     |           |
| tsn_app     | hyperram_release           | frdmimxrt1186      | cm33     |           |
| tsn_app     | ram_release                | frdmimxrt1186      | cm33     |           |
| tsn_app     | release_hybrid             | frdmimxrt1186      | cm33     | examples/_boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/core/prj_hybrid.conf |
| tsn_app     | ram_release_hybrid         | frdmimxrt1186      | cm33     | examples/_boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/core/prj_hybrid.conf |
| tsn_app     | release                    | frdmimxrt1186      | cm7      |           |
| tsn_app     | ram_release                | frdmimxrt1186      | cm7      |           |
| tsn_app     | release_motor_controller   | frdmimxrt1186      | cm7      | examples/_boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf |
| tsn_app     | ram_release_motor_controller | frdmimxrt1186    | cm7      | examples/_boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf |

**MCX E247**

For the MCX E31B one example application is provided: gPTP Endpoint. The application can be built in “release” mode.  

| **app**            | **config** | **board**    | **core**  | **file** |
|-------------|------------------------|--------------------|-----------------------|--------------------------|
| tsn_app     | release                | frdmmcxe247        |                       |                          |

**MCX E31B**

For the MCX E31B one example application is provided: TSN Endpoint. The application can be built in “release” mode.

| **app**            | **config** | **board**    | **core**  | **file** |
|-------------|------------------------|--------------------|-----------------------|--------------------------|
| tsn_app     | release                | frdmmcxe31b        |                       |                          |

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
