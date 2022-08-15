/* rv64_sifive_u.c
 */

#include <stdint.h>
#include "rv64_sifive_u.h"

/*
 * Entry function
 */
void __attribute__((naked,section(".init"))) _reset(void) {
    register uint32_t *src, *dst;

    /* save fdt address */
    asm volatile("mv s0, a1");

    /* hart id */
    uint32_t hartid;
    asm volatile("csrr %0, mhartid" : "=r"(hartid));
    asm volatile("mv tp, %0" :: "r"(hartid));

    /* set gp, sp */
    asm volatile("la gp, _global_pointer");
    asm volatile("la sp, _end_stack");
    asm volatile("slli t0, a0, %0" :: "i"(STACK_SHIFT));
    asm volatile("sub sp, sp, t0");

    /* setup single mode interrupt hundler */
    asm volatile("la t0, trap_start");
    asm volatile("csrw mtvec, t0");

    asm volatile("csrw mie, zero");
    asm volatile("li t0, %0" :: "i"(MIE_MSIE));
    asm volatile("csrs mie, t0");

    /* stop hart except hart 0 */
    if (hartid != 0) {
        hart_wfi_loop();
    }

    src = (uint32_t *) &_stored_data;
    dst = (uint32_t *) &_start_data;
    /* Copy the .data section from flash to RAM. */
    while (dst < (uint32_t *)&_end_data) {
        *dst = *src;
        dst++;
        src++;
    }

    /* zero clear bss section */
    dst = (uint32_t *) &_start_bss;
    while (dst < (uint32_t *) &_end_bss) {
        *dst = 0U;
        dst++;
    } 

    /* keep dtb addr in variable */
    asm volatile("mv %0, s0" : "=r"(qemu_dtb_addr));

    /* call main func */
    main();
    for(;;);
}

void do_boot(uint32_t *load_addr, uint32_t *dts_addr) {
    /* Trigger IPI for other waiting core to run SBI */
    uint64_t hartid = 0;
    asm volatile("mv %0, tp" : "=r"(hartid));
    if (hartid == 0) {
        ipi_global_data.type = BOOT_SYNC;
        ipi_global_data.arg1 = load_addr;
        ipi_global_data.arg2 = dts_addr;
        send_ipi_other(hartid);
    }

    /*
     * SBI entry's arguments
     * hartid, dts_address(, struct dynamic_info (for fw_dynamic))
     */
    asm volatile("mv a0, %0" :: "r"(hartid) : "a0");
    /* load DTS address to a1 */
    asm volatile("mv a1, %0" :: "r"((uint32_t *) dts_addr) : "a1");
    /* jump to firmware */
    asm volatile("jr %0" :: "r"((uint32_t *)(load_addr)));
}

/*
 * trap handler 
 */

void __attribute__((weak)) external_interrupt(void) {
    
}

void __attribute__((weak)) timer_interrupt(void) {

}

uint64_t handle_trap(uint64_t cause, uint64_t epc, uint64_t tval, struct stored_regs *regs) {
    uint64_t is_irq, irq;
    is_irq = (cause & MCAUSE_INT);
    irq = (cause & ~MCAUSE_INT);
    if (is_irq) {
        switch(irq) {
            case IRQ_M_EXT:
                external_interrupt();
                break;
            case IRQ_M_TIMER:
                timer_interrupt();
                break;
            default:
                break;
        }
    } else {
        /* handle exception */
    }

    return epc;
}

/*
 * IPI Handler
 */

void handle_ipi(uint64_t hartid) {
    if (hartid >= HART_NUM) return;

    /* clear ipi register */
    wmb();
    (*(volatile uint32_t *) MSIP_REG(hartid)) = 0;

    switch (ipi_global_data.type) {
        case BOOT_SYNC:
            do_boot(ipi_global_data.arg1, ipi_global_data.arg2);
            break;
        case REBOOT_SYNC:
            reboot();
            break;
        default:
            while(1)
                ;
    }
}

void send_ipi_other(uint64_t hartid) {
    uint64_t i;
    for (i = 0; i < HART_NUM; i++) {
        if (i == hartid) continue;
        /* send ipi (write regs) */
        wmb();
        (*(volatile uint32_t *) MSIP_REG(i)) = 1;
    }
}
