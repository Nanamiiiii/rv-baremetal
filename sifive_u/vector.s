# macros
.equ MAX_HARTS, 5
.equ STACK_SIZE, 1024
.equ REGBYTES, 8
.equ REG_NUM, 32
.equ CTX_SIZE, (REGBYTES * REG_NUM)
.equ MIE_MSIE, 0x8

# load from sp + (offset * register size)
.macro lxsp a, b
    ld \a, ((\b) * REGBYTES)(sp)
.endm

# store to sp + (offset * register size)
.macro sxsp a, b
    sd \a, ((\b) * REGBYTES)(sp)
.endm

# save registers
# Target: x0-x31
.macro trap_entry
    addi sp, sp, -CTX_SIZE
    sxsp x1, 1
    sxsp x2, 2
    sxsp x3, 3 
    sxsp x4, 4
    sxsp x5, 5
    sxsp x6, 6
    sxsp x7, 7
    sxsp x8, 8
    sxsp x9, 9
    sxsp x10, 10
    sxsp x11, 11
    sxsp x12, 12
    sxsp x13, 13
    sxsp x14, 14
    sxsp x15, 15
    sxsp x16, 16
    sxsp x17, 17
    sxsp x18, 18
    sxsp x19, 19
    sxsp x20, 20
    sxsp x21, 21
    sxsp x22, 22
    sxsp x23, 23
    sxsp x24, 24
    sxsp x25, 25
    sxsp x26, 26
    sxsp x27, 27
    sxsp x28, 28
    sxsp x29, 29
    sxsp x30, 30
    sxsp x31, 31
.endm

# load registers
# Target: ra, t0-t6, a0-a7
.macro trap_exit
    lxsp x1, 1
    lxsp x2, 2
    lxsp x3, 3 
    lxsp x4, 4
    lxsp x5, 5
    lxsp x6, 6
    lxsp x7, 7
    lxsp x8, 8
    lxsp x9, 9
    lxsp x10, 10
    lxsp x11, 11
    lxsp x12, 12
    lxsp x13, 13
    lxsp x14, 14
    lxsp x15, 15
    lxsp x16, 16
    lxsp x17, 17
    lxsp x18, 18
    lxsp x19, 19
    lxsp x20, 20
    lxsp x21, 21
    lxsp x22, 22
    lxsp x23, 23
    lxsp x24, 24
    lxsp x25, 25
    lxsp x26, 26
    lxsp x27, 27
    lxsp x28, 28
    lxsp x29, 29
    lxsp x30, 30
    lxsp x31, 31
    addi sp, sp, CTX_SIZE

    # Return
    mret
.endm

# isr vector
.section .isr_vector
.align 2

.global trap_start
trap_start:
    trap_entry

    csrr a0, mcause
    csrr a1, mepc
    csrr a2, mtval
    mv a3, sp
    jal handle_trap
    csrw mepc, a0

    trap_exit


    .text
    .global hart_wfi_loop
hart_wfi_loop:
    wfi

    csrr t0, mip
    andi t0, t0, MIE_MSIE
    beqz t0, hart_wfi_loop
    
    mv a0, tp
    jal handle_ipi

    j hart_wfi_loop


    .bss
    .align 4
    .global stacks
stacks:
    .skip STACK_SIZE * MAX_HARTS
