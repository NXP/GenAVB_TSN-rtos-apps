/*
* Copyright 2023 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef _FREERTOS_DSA_CONTROL_LPSPI_H_
#define _FREERTOS_DSA_CONTROL_LPSPI_H_

#include "FreeRTOS.h"
#include "semphr.h"

#include "fsl_lpspi.h"

#define DSA_CTRL_LPSPI_BASEADDR					LPSPI3
#define DSA_CTRL_LPSPI_SLAVE_IRQN				LPSPI3_IRQn
#define DSA_CTRL_LPSPI_SLAVE_PCS_FOR_INIT		kLPSPI_Pcs0
#define DSA_CTRL_LPSPI_SLAVE_PCS_FOR_TRANSFER	kLPSPI_SlavePcs0

/* When LPSPI is used as DSA control interface, a write/read access consists
 * of 2*32 bits control phase followed by a data phase of up to 64*32 bits.
 *
 * The first 32-bit word is defined as below:
 *
 * Bit[31]:
 *		Access Type (0: read, 1: write)
 *
 * Bits[30:16]:
 *		Write/Read Count
 *
 * Bits[15:0]:
 *		Command
 *
 * The second 32-bit word contains the parameters only used for read access.
 */

#define DSA_CTRL_LPSPI_ACCESS_TYPE_SHIFT		31
#define DSA_CTRL_LPSPI_ACCESS_TYPE_MASK			0x80000000
#define DSA_CTRL_LPSPI_ACCESS_TYPE_ENCODE(x)	(((x) << DSA_CTRL_LPSPI_ACCESS_TYPE_SHIFT) & \
														 DSA_CTRL_LPSPI_ACCESS_TYPE_MASK)
#define DSA_CTRL_LPSPI_ACCESS_TYPE_DECODE(x)	(((x) & DSA_CTRL_LPSPI_ACCESS_TYPE_MASK) >> \
														DSA_CTRL_LPSPI_ACCESS_TYPE_SHIFT)

#define DSA_CTRL_LPSPI_RW_COUNT_SHIFT			16
#define DSA_CTRL_LPSPI_RW_COUNT_MASK			0x7FFF0000
#define DSA_CTRL_LPSPI_RW_COUNT_ENCODE(x)		(((x) << DSA_CTRL_LPSPI_RW_COUNT_SHIFT) & \
														 DSA_CTRL_LPSPI_RW_COUNT_MASK)
#define DSA_CTRL_LPSPI_RW_COUNT_DECODE(x)		(((x) & DSA_CTRL_LPSPI_RW_COUNT_MASK) >> \
														DSA_CTRL_LPSPI_RW_COUNT_SHIFT)

#define DSA_CTRL_LPSPI_CMD_SHIFT				0
#define DSA_CTRL_LPSPI_CMD_MASK					0x0000FFFF
#define DSA_CTRL_LPSPI_CMD_ENCODE(x)			(((x) << DSA_CTRL_LPSPI_CMD_SHIFT) & \
														 DSA_CTRL_LPSPI_CMD_MASK)
#define DSA_CTRL_LPSPI_CMD_DECODE(x)			(((x) & DSA_CTRL_LPSPI_CMD_MASK) >> \
														DSA_CTRL_LPSPI_CMD_SHIFT)

struct dsa_ctrl_lpspi_slave {
	LPSPI_Type *base;
	lpspi_slave_handle_t drv_handle;
	status_t async_status;
	SemaphoreHandle_t mutex; /* Mutex to lock the handle during a trasfer */
	SemaphoreHandle_t event; /* Semaphore to notify and unblock task when transfer ends */
};

int dsa_ctrl_lpspi_recv(uint8_t *buf, uint32_t len);
int dsa_ctrl_lpspi_send(uint8_t *buf, uint32_t len);
int dsa_ctrl_lpspi_init(void);

#endif /* _FREERTOS_DSA_CONTROL_LPSPI_H_ */
