/*
 * Copyright 2023-2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_ele_base_api.h"

#include "board.h"
#include "avb_tsn/common/log.h"
#include "shared_config.h"
#include "avb_tsn/common/shell.h"
#include "avb_tsn/common/storage.h"
#include "trdc.h"

#define DEFAULT_M7_VTOR (BOARD_FLASH_START_ADDR + 0x100000)

#define M7_ITCM_ALIAS 0x203C0000
#define M7_ITCM_SIZE 0x00040000 /* 256KB */

void help_config_multicore(shell_handle_t shell)
{
    shell_printf(shell, "multicore config\n\n");
    shell_printf(shell, "path: /m7\n");
    shell_printf(shell, "parameters:\n");
    shell_printf(shell, "    boot:\n");
    shell_printf(shell, "        value: 0 or 1\n");
    shell_printf(shell, "        description: Set to 1 to boot M7 core, default 0\n");
    shell_printf(shell, "    vtor: (optional)\n");
    shell_printf(shell, "        description: M7 VTOR value, default 0x28100000 for boot from flash\n");
}

static int Start_CM7(void)
{
    /*
     * RT1180 Specific CM7 Kick Off operation
     */
    if (ELE_BaseAPI_EnableAPC(MU_RT_S3MUA) != kStatus_Success)
        goto err;

    /* Deassert Wait */
    BLK_CTRL_S_AONMIX->M7_CFG =
        (BLK_CTRL_S_AONMIX->M7_CFG & (~BLK_CTRL_S_AONMIX_M7_CFG_WAIT_MASK)) | BLK_CTRL_S_AONMIX_M7_CFG_WAIT(0);

    return 0;

err:
    return -1;
}

static void Prepare_landing_zone(uint32_t m7_vtor)
{
    log_info("Making Landing Zone at 0x%08X\n", m7_vtor);

    /* Write busy loop at the reset handler */
    *((volatile uint32_t *)(M7_ITCM_ALIAS + m7_vtor))     = 0x20000000;
    *((volatile uint32_t *)(M7_ITCM_ALIAS + m7_vtor + 4)) = 0x00000009;
    *((volatile uint32_t *)(M7_ITCM_ALIAS + m7_vtor + 8)) = 0xE7FEE7FE;
}

int multicore_init(void)
{
    if (!storage_cd("/m7", true)) {
        bool boot_m7 = false;
        uint32_t m7_vtor = DEFAULT_M7_VTOR;

        storage_read_bool("boot", &boot_m7);
        storage_read_u32("vtor", &m7_vtor);

        storage_cd("-", true);

        if (boot_m7) {
            bool boot_from_itcm;

            m7_vtor = m7_vtor & BLK_CTRL_S_AONMIX_M7_CFG_INITVTOR_MASK;

            boot_from_itcm = m7_vtor < M7_ITCM_SIZE;

            log_info("Booting M7, vtor: 0x%08X\n", m7_vtor);

            Prepare_CM7(m7_vtor);

            shared_system_config_set();

            if (boot_from_itcm) {
                Prepare_landing_zone(m7_vtor);
            } else {
                /* boot from flash */
                if (trdc_init() < 0) {
                    /* Failure is normal if M33 booted from ram */
                    log_err("Failed to configure TRDC\n");
                }
            }

            if (Start_CM7() < 0) {
                log_err("Failed to boot M7\n");
                goto err;
            }

            log_info("Done\n");
        }

    }

    return 0;

err:

    return -1;
}
