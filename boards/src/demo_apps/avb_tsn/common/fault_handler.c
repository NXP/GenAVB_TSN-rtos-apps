/*
 * Copyright 2022 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

enum { r0,
       r1,
       r2,
       r3,
       r12,
       lr,
       pc,
       psr };

/*!
 * Fault handling functions
 * See ARMv7-M Architecture Reference Manual:
 *      - chapter B1.5 ARMv7-M exception model: for more general informations
 *      - chapter B3.2.15 Configurable Fault Status Register: to interpret fault status
 * See ARMv8-M Architecture Reference Manual:
 *      - chapter B3.12 Faults
 */
static void MemManage_Fault(uint32_t stack[]) __attribute__((noreturn));
static void BusFault(uint32_t stack[]) __attribute__((noreturn));
static void UsageFault(uint32_t stack[]) __attribute__((noreturn));
static void HardFault(uint32_t stack[]) __attribute__((noreturn));
void Fault_Dispatcher(uint32_t *stack, uint32_t number) __attribute__((noreturn));

void Fault_Dispatcher(uint32_t *stack, uint32_t number)
{
    /* debugger can easly show local variables */
    volatile uint32_t R0;
    volatile uint32_t R1;
    volatile uint32_t R2;
    volatile uint32_t R3;
    volatile uint32_t R12;
    volatile uint32_t LR;  /* Link register. */
    volatile uint32_t PC;  /* Program counter. */
    volatile uint32_t PSR; /* Program status register. */

    R0 = stack[r0];
    R1 = stack[r1];
    R2 = stack[r2];
    R3 = stack[r3];
    R12 = stack[r12];
    LR = stack[lr];
    PC = stack[pc];
    PSR = stack[psr];

    (void)R0;
    (void)R1;
    (void)R2;
    (void)R3;
    (void)R12;
    (void)LR;
    (void)PC;
    (void)PSR;

    /* Since we bypass the console buffer, add a new line for readability */
    DbgConsole_BlockingPrintf("\n");

    switch (number) {
    case 3:
        HardFault(stack);
    case 4:
        MemManage_Fault(stack);
    case 5:
        BusFault(stack);
    case 6:
        UsageFault(stack);
    case 7:
        /* SecureFault */
        break;
    default:
        break;
    }

    for (;;)
        ;
}

static void StackDump(uint32_t stack[])
{
    DbgConsole_BlockingPrintf("r0  = 0x%08lx\n", stack[r0]);
    DbgConsole_BlockingPrintf("r1  = 0x%08lx\n", stack[r1]);
    DbgConsole_BlockingPrintf("r2  = 0x%08lx\n", stack[r2]);
    DbgConsole_BlockingPrintf("r3  = 0x%08lx\n", stack[r3]);
    DbgConsole_BlockingPrintf("r12 = 0x%08lx\n", stack[r12]);
    DbgConsole_BlockingPrintf("lr  = 0x%08lx\n", stack[lr]);
    DbgConsole_BlockingPrintf("pc  = 0x%08lx\n", stack[pc]);
    DbgConsole_BlockingPrintf("psr = 0x%08lx\n", stack[psr]);
}

static void MemManage_Fault(uint32_t stack[])
{
    volatile uint8_t MMFSR;  /* MemManage Status Register */
    volatile uint32_t MMFAR; /* MemManage Fault Address Register */

    MMFSR = SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos;
    MMFAR = SCB->MMFAR; /* address of the memory location that caused an MMU fault */

    DbgConsole_BlockingPrintf("MemManage Fault\n");
    DbgConsole_BlockingPrintf("MMFSR: 0x%08lx\n", MMFSR);
    DbgConsole_BlockingPrintf("MMFAR: 0x%08lx\n", MMFAR);

    StackDump(stack);
    for (;;)
        ;
}

static void BusFault(uint32_t stack[])
{
    volatile uint8_t BFSR;  /* BusFault Status Register */
    volatile uint32_t BFAR; /* BusFault Address Register */

    BFSR = SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos;
    BFAR = SCB->BFAR; /* address associated with a precise data access fault */

    DbgConsole_BlockingPrintf("Bus Fault\n");
    DbgConsole_BlockingPrintf("BFSR: 0x%08lx\n", BFSR);
    DbgConsole_BlockingPrintf("BFAR: 0x%08lx\n", BFAR);

    StackDump(stack);
    for (;;)
        ;
}

static void UsageFault(uint32_t stack[])
{
    volatile uint16_t UFSR; /* UsageFault Status Register */

    UFSR = SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos;

    DbgConsole_BlockingPrintf("Usage Fault\n");
    DbgConsole_BlockingPrintf("UFSR: 0x%08lx\n", UFSR);

    StackDump(stack);
    for (;;)
        ;
}

static void HardFault(uint32_t stack[])
{
    volatile uint32_t HFSR; /* HardFault Status Register */

    HFSR = SCB->HFSR;

    DbgConsole_BlockingPrintf("Hard Fault\n");
    DbgConsole_BlockingPrintf("HFSR: 0x%08lx\n", HFSR);

    /* FORCED Mask indicates that a fault with configurable priority has been escalated to a HardFault exception */
    if (HFSR & SCB_HFSR_FORCED_Msk) {
        if (SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk)
            MemManage_Fault(stack);

        if (SCB->CFSR & SCB_CFSR_BFARVALID_Msk)
            BusFault(stack);

        if (SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk)
            UsageFault(stack);
    }

    StackDump(stack);
    for (;;)
        ;
}
