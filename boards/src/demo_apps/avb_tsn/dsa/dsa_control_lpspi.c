/*
* Copyright 2023-2025 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"

#include "fsl_lpspi.h"

#include "genavb/error.h"

#include "avb_tsn/common/log.h"
#include "dsa_control_lpspi.h"

struct dsa_ctrl_lpspi_slave lpspi_slave;


static void dsa_ctrl_lpspi_slave_callback(LPSPI_Type *base, lpspi_slave_handle_t *handle, status_t status, void *userData)
{
	struct dsa_ctrl_lpspi_slave *slave_handle = (struct dsa_ctrl_lpspi_slave *)userData;
	BaseType_t reschedule = 0;

	slave_handle->async_status = status;
	xSemaphoreGiveFromISR(slave_handle->event, &reschedule);
	portYIELD_FROM_ISR(reschedule);
}

static int dsa_ctrl_lpspi_transfer(uint8_t *buf, uint32_t len, bool tx)
{
	lpspi_transfer_t slave_transfer;
	BaseType_t ret;
	status_t status;
	int rc;

	if (tx) {
		slave_transfer.txData = buf;
		slave_transfer.rxData = NULL;
	} else {
		slave_transfer.txData = NULL;
		slave_transfer.rxData = buf;
	}

	slave_transfer.dataSize = len;
	slave_transfer.configFlags = DSA_CTRL_LPSPI_SLAVE_PCS_FOR_TRANSFER | kLPSPI_SlaveByteSwap;

	/* Lock resource mutex */
	ret = xSemaphoreTake(lpspi_slave.mutex, portMAX_DELAY);
	if (ret != pdTRUE)
	{
		log_err("xSemaphoreTake() failed - LPSPI transfer is busy\n");
		rc = GENAVB_ERR_INVALID;
		goto err_mutex;
	}

	status = LPSPI_SlaveTransferNonBlocking(lpspi_slave.base, &lpspi_slave.drv_handle, &slave_transfer);
	if (status != kStatus_Success)
	{
		log_err("LPSPI_SlaveTransferNonBlocking() failed\n");
		rc = GENAVB_ERR_INVALID;
		goto err_xfer;
	}

	/* Wait for transfer to finish */
	ret = xSemaphoreTake(lpspi_slave.event, portMAX_DELAY);
	if (ret != pdTRUE)
	{
		log_err("xSemaphoreTake() failed - LPSPI driver error\n");
		rc = GENAVB_ERR_INVALID;
		goto err_sem;
	}

	/* Unlock resource mutex */
	(void)xSemaphoreGive(lpspi_slave.mutex);

	/* Return status captured by callback function */
	return lpspi_slave.async_status;

err_xfer:
err_sem:
	(void)xSemaphoreGive(lpspi_slave.mutex);
err_mutex:
	return rc;
}

int dsa_ctrl_lpspi_recv(uint8_t *buf, uint32_t len)
{
	int rc;

	rc = dsa_ctrl_lpspi_transfer(buf, len, false);
	if (rc < 0) {
		log_err("dsa_ctrl_lpspi_transfer() rx failed\n");
		goto err;
	}

	return GENAVB_SUCCESS;

err:
	return rc;
}

int dsa_ctrl_lpspi_send(uint8_t *buf, uint32_t len)
{
	int rc;

	rc = dsa_ctrl_lpspi_transfer(buf, len, true);
	if (rc < 0) {
		log_err("dsa_ctrl_lpspi_transfer() tx failed\n");
		goto err;
	}

	return GENAVB_SUCCESS;

err:
	return rc;
}

int dsa_ctrl_lpspi_init(void)
{
	lpspi_slave_config_t slave_config;

	memset(&lpspi_slave, 0, sizeof(struct dsa_ctrl_lpspi_slave));

	lpspi_slave.mutex = xSemaphoreCreateMutex();
	if (!lpspi_slave.mutex) {
		log_err("xSemaphoreCreateMutex() failed\n");
		goto err_mutex;
	}

	lpspi_slave.event = xSemaphoreCreateBinary();
	if (!lpspi_slave.event) {
		log_err("xSemaphoreCreateBinary() failed\n");
		goto err_event;
	}

	lpspi_slave.base = DSA_CTRL_LPSPI_BASEADDR;

	LPSPI_SlaveGetDefaultConfig(&slave_config);
	slave_config.bitsPerFrame = 8;
	slave_config.whichPcs = DSA_CTRL_LPSPI_SLAVE_PCS_FOR_INIT;

	NVIC_SetPriority(DSA_CTRL_LPSPI_SLAVE_IRQN, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2);
	LPSPI_SlaveInit(lpspi_slave.base, &slave_config);
	LPSPI_SlaveTransferCreateHandle(lpspi_slave.base, &lpspi_slave.drv_handle, &dsa_ctrl_lpspi_slave_callback, (void *)&lpspi_slave);

	return GENAVB_SUCCESS;

err_event:
	vSemaphoreDelete(lpspi_slave.mutex);
err_mutex:
	return GENAVB_ERR_INVALID;
}
