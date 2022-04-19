# macros
.equ MAX_HARTS, 4
.equ STACK_SIZE, 1024
.equ REGBYTES, 8
.equ REG_NUM, 16
.equ CTX_SIZE, (REGBYTES * REG_NUM)

# load from sp + (offset * register size)
.macro lxsp a, b
    ld \a, ((\b) * REGBYTES)(sp)
.endm

# store to sp + (offset * register size)
.macro sxsp a, b
    sd \a, ((\b) * REGBYTES)(sp)
.endm

# save registers
# Target: ra, t0-t6, a0-a7
.macro trap_entry
    addi sp, sp, -CTX_SIZE
    sxsp x1, 0
    sxsp x5, 1
    sxsp x6, 2
    sxsp x7, 3
    sxsp x10, 4
    sxsp x11, 5
    sxsp x12, 6
    sxsp x13, 7
    sxsp x14, 8
    sxsp x15, 9
    sxsp x16, 10
    sxsp x17, 11
    sxsp x28, 12
    sxsp x29, 13
    sxsp x30, 14
.endm

# load registers
# Target: ra, t0-t6, a0-a7
.macro trap_exit
    lxsp x1, 0
    lxsp x5, 1
    lxsp x6, 2
    lxsp x7, 3
    lxsp x10, 4
    lxsp x11, 5
    lxsp x12, 6
    lxsp x13, 7
    lxsp x14, 8
    lxsp x15, 9
    lxsp x16, 10
    lxsp x17, 11
    lxsp x28, 12
    lxsp x29, 13
    lxsp x30, 14
    addi sp, sp, CTX_SIZE

    # Return
    mret
.endm

# isr vector
.section .isr_vector
.align 8

.global IV

IV:
    j _synctrap
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_vmsi
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_vmti
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_vmei
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_irq0
    .align 2
    j trap_irq1
    .align 2
    j trap_irq2
    .align 2
    j trap_irq3
    .align 2
    j trap_irq4
    .align 2
    j trap_irq5
    .align 2
    j trap_irq6
    .align 2
    j trap_irq7
    .align 2
    j trap_irq8
    .align 2
    j trap_irq9
    .align 2
    j trap_irq10
    .align 2
    j trap_irq11
    .align 2
    j trap_irq12
    .align 2
    j trap_irq13
    .align 2
    j trap_irq14
    .align 2
    j trap_irq15
    .align 2

_synctrap:
    trap_entry
    jal isr_synctrap
    trap_exit

trap_vmsi:
    trap_entry
    jal isr_vmsi
    trap_exit

trap_vmti:
    trap_entry
    jal isr_vmti
    trap_exit

trap_vmei:
    trap_entry
    jal isr_vmei
    trap_exit

trap_irq0:
    trap_entry
    jal isr_irq0
    trap_exit

trap_irq1:
    trap_entry
    jal isr_irq1
    trap_exit

trap_irq2:
    trap_entry
    jal isr_irq2
    trap_exit

trap_irq3:
    trap_entry
    jal isr_irq3
    trap_exit

trap_irq4:
    trap_entry
    jal isr_irq4
    trap_exit

trap_irq5:
    trap_entry
    jal isr_irq5
    trap_exit

trap_irq6:
    trap_entry
    jal isr_irq6
    trap_exit

trap_irq7:
    trap_entry
    jal isr_irq7
    trap_exit

trap_irq8:
    trap_entry
    jal isr_irq8
    trap_exit

trap_irq9:
    trap_entry
    jal isr_irq9
    trap_exit

trap_irq10:
    trap_entry
    jal isr_irq10
    trap_exit

trap_irq11:
    trap_entry
    jal isr_irq11
    trap_exit

trap_irq12:
    trap_entry
    jal isr_irq12
    trap_exit

trap_irq13:
    trap_entry
    jal isr_irq13
    trap_exit

trap_irq14:
    trap_entry
    jal isr_irq14
    trap_exit

trap_irq15:
    trap_entry
    jal isr_irq15
    trap_exit

trap_empty:
    nop


    .bss
    .align 4
    .global stacks
stacks:
    .skip STACK_SIZE * MAX_HARTS