#include <stdint.h>
#include "macros.h"

#define HART_NUM 5
#define STACK_SHIFT 10

#define MCAUSE_INT 0x8000000000000000

#define IRQ_M_SOFT      3
#define IRQ_M_TIMER     7
#define IRQ_M_EXT       11

#define MIE_MSIE (0x1UL << IRQ_M_SOFT)

extern void trap_entry(void);
extern void trap_exit(void);
extern void trap_start(void);
extern void hart_wfi_loop(void);

extern uint32_t  _start_vector;
extern uint32_t  _stored_data;
extern uint32_t  _start_data;
extern uint32_t  _end_data;
extern uint32_t  _start_bss;
extern uint32_t  _end_bss;
extern uint32_t  _end_stack;
extern uint32_t  _start_heap;
extern uint32_t  _global_pointer;

struct stored_regs {
    unsigned long sepc;
    unsigned long ra;
    unsigned long sp;
    unsigned long gp;
    unsigned long tp;
    unsigned long t0;
    unsigned long t1;
    unsigned long t2;
    unsigned long s0;
    unsigned long s1;
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
    unsigned long a4;
    unsigned long a5;
    unsigned long a6;
    unsigned long a7;
    unsigned long s2;
    unsigned long s3;
    unsigned long s4;
    unsigned long s5;
    unsigned long s6;
    unsigned long s7;
    unsigned long s8;
    unsigned long s9;
    unsigned long s10;
    unsigned long s11;
    unsigned long t3;
    unsigned long t4;
    unsigned long t5;
    unsigned long t6;
    unsigned long status;
    unsigned long badaddr;
    unsigned long cause;
};

struct ipi_boot {
    uint32_t *boot_addr;
    uint32_t *dts_addr;
};

struct ipi_boot ipi_boot_arg;

void send_ipi_other(uint64_t hartid);

uint32_t * volatile qemu_dtb_addr;

extern void main(void);

void __attribute__((naked,section(".init"))) _reset(void) {
    register uint32_t *src, *dst;

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

    /* keep dtb address in stack */
    asm volatile("sd a1, -8(sp)");

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

    asm volatile("ld %0, -8(sp)" : "=r"(qemu_dtb_addr));

    // call main func
    main();
    for(;;);
}

void do_boot(uint32_t *load_addr, uint32_t *dts_addr) {
    /* Trigger IPI for other waiting core to run SBI */
    uint64_t hartid = 0;
    asm volatile("mv %0, tp" : "=r"(hartid));
    if (hartid == 0) {
        ipi_boot_arg.boot_addr = load_addr;
        ipi_boot_arg.dts_addr = dts_addr;
        send_ipi_other(hartid);
    }

    /*
     * SBI entry's arguments
     * hartid, dts_address(, struct dynamic_info (for fw_dynamic))
     */
    asm volatile("mv a0, %0" :: "r"(hartid));
    /* load DTS address to a1 */
    asm volatile("mv a1, %0" :: "r"((uint32_t *) dts_addr));
    /* jump to firmware */
    asm volatile("jr %0" :: "r"((uint32_t *)(load_addr)));
}

void RAMFUNCTION arch_reboot(void) {
    while(1);
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

#define ASM_FENCE(p, s) \
    asm volatile ("fence " #p "," #s ::: "memory")

#define wmb() ASM_FENCE(ow,ow)
#define MSIP_BASE 0x2000000UL
#define MSIP_REG(hart) ((uint64_t)(MSIP_BASE) + (hart) * 4)

void handle_ipi(uint64_t hartid) {
    if (hartid >= HART_NUM) return;

    /* clear ipi register */
    wmb();
    (*(volatile uint32_t *) MSIP_REG(hartid)) = 0;

    do_boot(ipi_boot_arg.boot_addr, ipi_boot_arg.dts_addr);
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
