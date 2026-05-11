# GenAVB/TSN RTOS Applications

GenAVB/TSN stack reference applications for RTOS covering TSN use cases, running on i.MX RT 4-digit and MCX E series family of devices.

## Overview

This repository provides a comprehensive set of TSN example applications demonstrating Time-Sensitive Networking (TSN) features

## Supported Hardware

This release supports the following boards:

| SoC            | Board Name        | Description                    |
|----------------|-------------------|--------------------------------|
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

#### GNU ARM Embedded Toolchain and Host Tools

GNU ARM Toolchain and Host tools can be installed using the [MCUXpresso Installer from nxp.com](https://www.nxp.com/design/design-center/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-installer:MCUXPRESSO-INSTALLER) utility for both Linux and Windows Hosts. 

From the NXP MCUXpresso Installer main menu select and install the following components:
* MCUXpresso SDK Developper v26.03
* Arm GNU Toolchain v14.2.1
* MCUXpresso Secure Provisioning Tool v26.03 (Optional)

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
west init -m https://github.com/NXP/GenAVB_TSN-rtos-apps <workspace> --mr %tag%
cd <workspace>\GenAVB_TSN-rtos-apps
```

## Repository Structure

```
GenAVB_TSN-rtos-apps/
├── boards/
│   ├── src/                          # Board-agnostic application source code
│   │   └── demo_apps/
│   │       └── avb_tsn/
│   │           ├── tsn_app/          # TSN networking application
│   │           ├── dsa_enetc/        # DSA ENETC application
│   │           ├── dsa_switch/       # DSA switch application
│   │           └── common/           # Application common code
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

The bootstrap script automates the following tasks:
- Update the west workspace with all required repositories
- Apply required patches to MCUX SDK when present
- Prepare the environment for application builds

**Linux:**
```bash
cd <workspace>/GenAVB_TSN-rtos-apps/scripts
source bootstrap.sh
```

**Windows:**
```cmd
cd <workspace>\GenAVB_TSN-rtos-apps\scripts
bootstrap.bat
```

After bootstrap completes, the environment is ready to build applications.

### Netconf Support prerequisites
GenAVB/TSN release provides Netconf support for i.MX RT1180 board. To enable Netconf support, ensure the following prerequisites are met:

Download Netconf additionals packages (sysrepo-genavb , mcux-yang and mcux-netconf) from nxp.com or contact NXP support.

Install sysrepo-genavb sources in the west workspace (Linux):

```cmd
cd <workspace>/mcuxsdk/components
mkdir sysrepo-genavb && tar -xzf  sysrepo-genavb-<version>.tar.gz -C sysrepo-genavb --strip-components=1
```

Install mcux-yang sources in the workspace:

```cmd
cd <workspace>/mcuxsdk/components
mkdir mcux-yang && tar -xzf mcux-yang-<version>.tar.gz  -C mcux-yang --strip-components=1
```

Install mcux-netconf sources in the workspace:

```cmd
cd <workspace>
mkdir mcux-netconf && tar -xzf mcux-netconf-<version>.tar.gz  -C mcux-netconf --strip-components=1
```
#### Repository Structure (with Netconf support)
```
<workspace>/
├── mcuxsdk/
│   └── components/ 
│       │── sysrepo-genavb/
│       └── mcux-yang/   
│   
├── mcux-netconf/
└── GenAVB_TSN-rtos-apps/
```

### Building Applications

Run the `west build` command from the repository root. The generic command format is:

```bash
west build -p always boards/src/demo_apps/avb_tsn/<app> --toolchain armgcc --config <config> -b <board> [-Dcore_id=<core>] [-DCONF_FILE=<file>]
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

**i.MX RT1170**

For i.MX RT1170 three example applications are provided: TSN Endpoint, AVB Endpoint, and Milan AVB Endpoint. Each application can be built in “release” or “ram_release” mode. The “release” images boot from external QuadSPI NOR flash and then relocate to internal memory (Code TCM, System TCM and OCRAM). The “ram_release” images boot from internal memory.

For TSN Endpoint application, the “sdram_release” mode is also available, and it boots from external QuadSPI NOR flash and then relocates to internal memory (Code TCM, System TCM, OCRAM and Sdram).


| **app**            | **config**             | **board**          | **core**                 | **file**  |
|--------------------|------------------------|--------------------|--------------------------|--------------------------|
| tsn_app            | release                | evkbmimxrt1170     | cm7                      |                          |
| tsn_app            | ram_release            | evkbmimxrt1170     | cm7                      |                          |
| tsn_app            | sdram_release          | evkbmimxrt1170     | cm7                      |                          |
| tsn_app            | release_motor          | evkbmimxrt1170     | cm7                      | boards/evkbmimxrt1170/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf;boards/evkbmimxrt1170/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf |
| tsn_app            | ram_release_motor      | evkbmimxrt1170     | cm7                      | boards/evkbmimxrt1170/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf;boards/evkbmimxrt1170/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf |

**i.MX RT1180**

For i.MX RT1180 TSN applications, two example applications with three supported configurations are provided: TSN Bridge, TSN Endpoint and a combined TSN Bridge + Endpoint, which run respectively on Cortex-M33, Cortex-M7 and Cortex-M33 cores. Each application can be built in “release”, “hyperram_release” or “ram_release” mode. The “release” images boot from external QuadSPI NOR flash and then relocate to internal memory (Code TCM, System TCM and OCRAM). The hyperram_release boots from external QuadSPI NOR flash and then relocates to internal memory (Code TCM, System TCM, OCRAM and Hyperram). The “ram_release” images boot from internal memory.

For the TSN Bridge application, two configurations are available (combined with the build mode).

For the TSN Endpoint application, four configurations are available (combined with the build mode).

For i.MX RT1180 DSA applications, two example applications are provided: DSA ENETC CPU port and DSA Switch CPU port, both of which running on Cortex-M33 core. Each application can be built in “release” or “ram_release” mode, following the same boot sequence as the previous section’s description.

For i.MX RT1180 Netconf application, on example application is provided and built in "hyperram_release" mode only.

| **app**     | **config**                 | **board**          | **core** | **file**  |
|-------------|----------------------------|--------------------|----------|-----------|
| tsn_app     | release                    | evkmimxrt1180      | cm33     |           |
| tsn_app     | hyperram_release           | evkmimxrt1180      | cm33     | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm33/prj_netconf.conf |
| tsn_app     | ram_release                | evkmimxrt1180      | cm33     |           |
| tsn_app     | release_no_enetc0          | evkmimxrt1180      | cm33     | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm33/prj_no_enetc0.conf |
| tsn_app     | ram_release_no_enetc0      | evkmimxrt1180      | cm33     | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm33/prj_no_enetc0.conf |
| tsn_app     | release_hybrid             | evkmimxrt1180      | cm33     | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm33/prj_hybrid.conf |
| tsn_app     | ram_release_hybrid         | evkmimxrt1180      | cm33     | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm33/prj_hybrid.conf |
| tsn_app     | release                    | evkmimxrt1180      | cm7      |           |
| tsn_app     | ram_release                | evkmimxrt1180      | cm7      |           |
| tsn_app     | release_enetc0             | evkmimxrt1180      | cm7      | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_enetc0.conf |
| tsn_app     | ram_release_enetc0         | evkmimxrt1180      | cm7      | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_enetc0.conf |
| tsn_app     | release_motor_controller   | evkmimxrt1180      | cm7      | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf |
| tsn_app     | ram_release_motor_controller | evkmimxrt1180    | cm7      | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf |
| tsn_app     | release_motor_iodevice     | evkmimxrt1180      | cm7      | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf |
| tsn_app     | ram_release_motor_iodevice | evkmimxrt1180      | cm7      | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf |
| dsa/dsa_enetc   | release                    | evkmimxrt1180      | cm33     |           |
| dsa/dsa_enetc   | ram_release                | evkmimxrt1180      | cm33     |           |
| dsa/dsa_switch  | release                    | evkmimxrt1180      | cm33     |           |
| dsa/dsa_switch  | ram_release                | evkmimxrt1180      | cm33     |           |

**i.MX RT1186**

For i.MX RT1186 TSN applications, two example applications with three supported configurations are provided: TSN Bridge, TSN Endpoint and a combined TSN Bridge + Endpoint, which run respectively on Cortex-M33, Cortex-M7 and Cortex-M33 cores. Each application can be built in “release”, “hyperram_release” or “ram_release” mode. The “release” images boot from external QuadSPI NOR flash and then relocate to internal memory (Code TCM, System TCM and OCRAM). The hyperram_release boots from external QuadSPI NOR flash and then relocates to internal memory (Code TCM, System TCM, OCRAM and Hyperram). The “ram_release” images boot from internal memory.

For the TSN Bridge application, two configurations are available (combined with the build mode).

For the TSN Endpoint application, two configurations are available (combined with the build mode).

| **app**     | **config**                 | **board**          | **core** | **file**  |
|-------------|----------------------------|--------------------|----------|-----------|
| tsn_app     | release                    | frdmimxrt1186      | cm33     |           |
| tsn_app     | hyperram_release           | frdmimxrt1186      | cm33     |           |
| tsn_app     | ram_release                | frdmimxrt1186      | cm33     |           |
| tsn_app     | release_hybrid             | frdmimxrt1186      | cm33     | boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/cm33/prj_hybrid.conf |
| tsn_app     | ram_release_hybrid         | frdmimxrt1186      | cm33     | boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/cm33/prj_hybrid.conf |
| tsn_app     | release                    | frdmimxrt1186      | cm7      |           |
| tsn_app     | ram_release                | frdmimxrt1186      | cm7      |           |
| tsn_app     | release_motor_controller   | frdmimxrt1186      | cm7      | boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf |
| tsn_app     | ram_release_motor_controller | frdmimxrt1186    | cm7      | boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf |

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
| tsn_app     | release_motor_iodevice | frdmmcxe31b        |                       | boards/frdmmcxe31b/demo_apps/avb_tsn/tsn_app/prj_motor_iodevice.conf |

## Environment Variables

When opening a new terminal session, use the MCUXpresso tools integrated terminal so the virtual environment is automatically set up, and ensure `ARMGCC_DIR` points to the Arm GNU toolchain installation.

### Flashing Image Booting

Note: on MIMXRT1170-EVKB and MIMXRT1180-EVK boards for both Jlink or LinkServer flashing methods, make sure the jumper JP5 is removed.

Run the `west flash` command from the application directory. The generic command format is:

```bash
cd <workspace>/GenAVB_TSN-rtos-apps/
west flash --runner <linkserver | jlink>  [-d <path to build directory>]
```