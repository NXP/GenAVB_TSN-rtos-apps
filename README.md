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
| MCX E32        | frdmmcxe32b       | MCX E32B Freedom Board         |

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
* MCUXpresso SDK Developer v26.09
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
│   │           ├── tsn_app/          # TSN networking application (single-core)
│   │           ├── tsn_app_multicore/ # TSN networking application (dual-core, sysbuild)
│   │           ├── dsa_enetc/        # DSA ENETC application
│   │           ├── dsa_switch/       # DSA switch application
│   │           └── common/           # Application common code
│   ├── evkbmimxrt1170/              # Board-specific files for RT1170
│   ├── evkmimxrt1180/               # Board-specific files for RT1180
│   ├── frdmimxrt1186/               # Board-specific files for RT1186
│   ├── frdmmcxe247/                 # Board-specific files for MCX E247
│   ├── frdmmcxe31b/                 # Board-specific files for MCX E31B
│   └── frdmmcxe32b/                 # Board-specific files for MCX E32B
├── devices/                          # SoC-specific shared files
├── scripts/                          # Setup scripts
│   ├── bootstrap.sh                 # Linux automated setup
│   └── bootstrap.bat                # Windows automated setup
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
| tsn_app            | motor_release          | evkbmimxrt1170     | cm7                      | boards/evkbmimxrt1170/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf;boards/evkbmimxrt1170/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf |
| tsn_app            | motor_ram_release      | evkbmimxrt1170     | cm7                      | boards/evkbmimxrt1170/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf;boards/evkbmimxrt1170/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf |

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
| tsn_app     | no_enetc0_release          | evkmimxrt1180      | cm33     | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm33/prj_no_enetc0.conf |
| tsn_app     | no_enetc0_ram_release      | evkmimxrt1180      | cm33     | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm33/prj_no_enetc0.conf |
| tsn_app     | hybrid_release             | evkmimxrt1180      | cm33     | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm33/prj_hybrid.conf |
| tsn_app     | hybrid_ram_release         | evkmimxrt1180      | cm33     | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm33/prj_hybrid.conf |
| tsn_app     | release                    | evkmimxrt1180      | cm7      |           |
| tsn_app     | ram_release                | evkmimxrt1180      | cm7      |           |
| tsn_app     | enetc0_release             | evkmimxrt1180      | cm7      | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_enetc0.conf |
| tsn_app     | enetc0_ram_release         | evkmimxrt1180      | cm7      | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_enetc0.conf |
| tsn_app     | motor_controller_release   | evkmimxrt1180      | cm7      | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf |
| tsn_app     | motor_controller_ram_release | evkmimxrt1180    | cm7      | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf |
| tsn_app     | motor_iodevice_release     | evkmimxrt1180      | cm7      | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf |
| tsn_app     | motor_iodevice_ram_release | evkmimxrt1180      | cm7      | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf |
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
| tsn_app     | hybrid_release             | frdmimxrt1186      | cm33     | boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/cm33/prj_hybrid.conf |
| tsn_app     | hybrid_ram_release         | frdmimxrt1186      | cm33     | boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/cm33/prj_hybrid.conf |
| tsn_app     | release                    | frdmimxrt1186      | cm7      |           |
| tsn_app     | ram_release                | frdmimxrt1186      | cm7      |           |
| tsn_app     | motor_controller_release   | frdmimxrt1186      | cm7      | boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf |
| tsn_app     | motor_controller_ram_release | frdmimxrt1186    | cm7      | boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf |

**MCX E247**

For the MCX E247 one example application is provided: gPTP Endpoint. The application can be built in “release” mode.

| **app**            | **config** | **board**    | **core**  | **file** |
|-------------|------------------------|--------------------|-----------------------|--------------------------|
| tsn_app     | release                | frdmmcxe247        |                       |                          |

**MCX E31B**

For the MCX E31B one example application is provided: TSN Endpoint. The application can be built in “release” mode.

| **app**            | **config** | **board**    | **core**  | **file** |
|-------------|------------------------|--------------------|-----------------------|--------------------------|
| tsn_app     | release                | frdmmcxe31b        |                       |                          |
| tsn_app     | motor_iodevice_release | frdmmcxe31b        |                       | boards/frdmmcxe31b/demo_apps/avb_tsn/tsn_app/prj_motor_iodevice.conf |
| tsn_app     | motor_controller_release | frdmmcxe31b        |                       | boards/frdmmcxe31b/demo_apps/avb_tsn/tsn_app/prj_motor_controller.conf |

**MCX E32B**

For the MCX E32B one example application is provided: TSN Endpoint. The application can be built in “release” mode.

| **app**            | **config** | **board**    | **core**  | **file** |
|-------------|------------------------|--------------------|-----------------------|--------------------------|
| tsn_app     | release                | frdmmcxe32b        |                       |                          |
| tsn_app     | motor_iodevice_release | frdmmcxe32b        |                       | boards/frdmmcxe32b/demo_apps/avb_tsn/tsn_app/prj_motor_iodevice.conf |
| tsn_app     | motor_controller_release | frdmmcxe32b        |                       | boards/frdmmcxe32b/demo_apps/avb_tsn/tsn_app/prj_motor_controller.conf |

### Multicore Build (CM33 + CM7)

The i.MX RT1180 and i.MX RT1186 are dual-core SoCs featuring a Cortex-M33 (primary/boot core)
and a Cortex-M7 (secondary core). The `tsn_app_multicore` application builds both cores
simultaneously using the west **sysbuild** framework: the primary Cortex-M33 image (TSN Bridge) 
drives the build and automatically pulls in the secondary Cortex-M7 image (TSN Endpoint, 
Motor Controller or Motor IO Device).

> **Note:** The CM33 is the boot master. It starts the CM7 core after board reset.

#### Supported boards

| SoC         | Board          | Primary core (CM33) | Secondary core (CM7)                            |
|-------------|----------------|---------------------|-------------------------------------------------|
| i.MX RT1180 | evkmimxrt1180  | TSN Bridge          | TSN Endpoint, Motor Controller, Motor IO Device |
| i.MX RT1186 | frdmimxrt1186  | TSN Bridge          | TSN Endpoint, Motor Controller                  |

#### Build command

The build is always launched from the **primary** application directory with `--sysbuild` and
`-Dcore_id=cm33`. The secondary (CM7) image is configured and built automatically.

Run the `west build` command from the repository root. The generic command format is:

```bash
west build -p always -d <build_dir> \
  boards/src/demo_apps/avb_tsn/tsn_app_multicore/primary \
  --sysbuild --toolchain armgcc --config <cm33_config> \
  -b <board> -Dcore_id=cm33 \
  [-DCONF_FILE=<cm33_conf_file>] \
  [-Dtsn_app_multicore_secondary_CMAKE_BUILD_TYPE=<cm7_config>] \
  [-Dtsn_app_multicore_secondary_CONF_FILE=<cm7_conf_file>]
```

The `-Dtsn_app_multicore_secondary_*` parameters are **optional**: when omitted, the CM7 builds
with its default `release` configuration (TSN Endpoint, default port assignment).

#### Build configurations

**i.MX RT1180 (`evkmimxrt1180`)**

_Primary core — CM33 (`--config <cm33_config>` and, when required, `-DCONF_FILE=<cm33_conf_file>`):_

| CM33 `--config`      | CM33 `CONF_FILE`                                                          | Description                          |
|----------------------|---------------------------------------------------------------------------|--------------------------------------|
| release            | *(none)*                                                                  | TSN Bridge, with ENETC0 (default)    |
| no_enetc0_release  | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm33/prj_no_enetc0.conf  | TSN Bridge, ENETC0 left to the CM7   |
| hybrid_release     | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm33/prj_hybrid.conf     | TSN Bridge + Endpoint (hybrid)       |

_Secondary core — CM7 (`-Dtsn_app_multicore_secondary_CMAKE_BUILD_TYPE=<cm7_config>` and, when required, `-Dtsn_app_multicore_secondary_CONF_FILE=<cm7_conf_file>`):_

| CM7 `_CMAKE_BUILD_TYPE`     | CM7 `_CONF_FILE`                                                                | Description                    |
|-----------------------------|---------------------------------------------------------------------------------|--------------------------------|
| *(default)*                 | *(none)*                                                                        | TSN Endpoint, default ports    |
| enetc0_release            | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_enetc0.conf            | TSN Endpoint, owns ENETC0      |
| motor_controller_release  | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf  | Motor Controller               |
| motor_iodevice_release    | boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_iodevice.conf    | Motor IO Device                |

> **Note:** The `no_enetc0_release` CM33 configuration and the `enetc0_release` CM7 configuration
> are meant to be used **together**: the CM33 gives up ENETC0 so the CM7 can own it. Using
> `enetc0_release` on the CM7 without `no_enetc0_release` (+ its `CONF_FILE`) on the CM33 makes
> both cores drive the ENETC0 PHY and the CM7 `PHY_Init()` fails.

**i.MX RT1186 (`frdmimxrt1186`)**

_Primary core — CM33:_

| CM33 `--config`  | CM33 `CONF_FILE`                                                        | Description                    |
|------------------|-------------------------------------------------------------------------|--------------------------------|
| release        | *(none)*                                                                | TSN Bridge (default)           |
| hybrid_release | boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/cm33/prj_hybrid.conf   | TSN Bridge + Endpoint (hybrid) |

_Secondary core — CM7:_

| CM7 `_CMAKE_BUILD_TYPE`    | CM7 `_CONF_FILE`                                                               | Description       |
|----------------------------|--------------------------------------------------------------------------------|-------------------|
| *(default)*                | *(none)*                                                                       | TSN Endpoint      |
| motor_controller_release | boards/frdmimxrt1186/demo_apps/avb_tsn/tsn_app/cm7/prj_motor_controller.conf | Motor Controller  |

#### Examples

Default multicore build (Bridge + Endpoint) for i.MX RT1180:

```bash
west build -p always -d build_rt1180_multicore_release \
  boards/src/demo_apps/avb_tsn/tsn_app_multicore/primary \
  --sysbuild --toolchain armgcc --config release \
  -b evkmimxrt1180 -Dcore_id=cm33
```

Bridge no-ENETC0 (CM33) + Endpoint ENETC0 (CM7) for i.MX RT1180:

```bash
west build -p always -d build_rt1180_multicore_enetc0 \
  boards/src/demo_apps/avb_tsn/tsn_app_multicore/primary \
  --sysbuild --toolchain armgcc --config no_enetc0_release \
  -b evkmimxrt1180 -Dcore_id=cm33 \
  -DCONF_FILE=boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm33/prj_no_enetc0.conf \
  -Dtsn_app_multicore_secondary_CMAKE_BUILD_TYPE=enetc0_release \
  -Dtsn_app_multicore_secondary_CONF_FILE=boards/evkmimxrt1180/demo_apps/avb_tsn/tsn_app/cm7/prj_enetc0.conf
```

Default multicore build for i.MX RT1186:

```bash
west build -p always -d build_rt1186_multicore_release \
  boards/src/demo_apps/avb_tsn/tsn_app_multicore/primary \
  --sysbuild --toolchain armgcc --config release \
  -b frdmimxrt1186 -Dcore_id=cm33
```

## Environment Variables

When opening a new terminal session, use the MCUXpresso tools integrated terminal so the virtual environment is automatically set up, and ensure `ARMGCC_DIR` points to the Arm GNU toolchain installation.

### Flashing Image Booting

Note: on MIMXRT1170-EVKB and MIMXRT1180-EVK boards for both Jlink or LinkServer flashing methods, make sure the jumper JP5 is removed.

Run the `west flash` command from the application directory. The generic command format is:

```bash
cd <workspace>/GenAVB_TSN-rtos-apps/
west flash --runner <linkserver | jlink>  [-d <path to build directory>]
```

> **Note:** If JLink runner is selected a board reset is required once the flash is completed.
