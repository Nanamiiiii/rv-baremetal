/* rv64_sifive_u.h
 */

#define RAM __attribute__((used,section(".ram")))
#define HART_NUM 5
#define STACK_SHIFT 10

#define MCAUSE_INT 0x8000000000000000

#define IRQ_M_SOFT      3
#define IRQ_M_TIMER     7
#define IRQ_M_EXT       11

#define MIE_MSIE (0x1UL << IRQ_M_SOFT)

#define ASM_FENCE(p, s) \
    asm volatile ("fence " #p "," #s ::: "memory")

#define wmb() ASM_FENCE(ow,ow)
#define MSIP_BASE 0x2000000UL
#define MSIP_REG(hart) ((uint64_t)(MSIP_BASE) + (hart) * 4)

extern void trap_start(void);
extern void hart_wfi_loop(void);
extern void reboot(void);
extern void main(void);

extern uint64_t  _start_vector;
extern uint64_t  _stored_data;
extern uint64_t  _start_data;
extern uint64_t  _end_data;
extern uint64_t  _start_bss;
extern uint64_t  _end_bss;
extern uint64_t  _end_stack;
extern uint64_t  _start_heap;
extern uint64_t  _global_pointer;

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

struct ipi_data {
    uint8_t type;
    uint32_t *arg1;
    uint32_t *arg2;
};

enum ipi_type {
    BOOT_SYNC,
    REBOOT_SYNC,
};

struct ipi_data ipi_global_data;
uint32_t * volatile qemu_dtb_addr;

void do_boot(uint32_t *load_addr, uint32_t *dts_addr);
uint64_t handle_trap(uint64_t cause, uint64_t epc, uint64_t tval, struct stored_regs *regs);
void handle_ipi(uint64_t hartid);
void send_ipi_other(uint64_t hartid);
