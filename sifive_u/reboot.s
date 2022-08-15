# resetting board virtually
    .section .ram
    .global reboot
reboot:
    bnez tp, reg_reset
    li t0, 0x1
    la t1, ipi_global_data
    sb t0, 0(t1) # REBOOT_SYNC
    mv a0, tp
    la t1, send_ipi_other
    jalr t1 
reg_reset:
    li sp, 0
    li gp, 0
    li tp, 0
    li t0, 0
    li t1, 0
    li t2, 0
    li s0, 0
    li s1, 0
    li a2, 0
    li a3, 0
    li a4, 0
    li a5, 0
    li a6, 0
    li a7, 0
    li s2, 0
    li s3, 0
    li s4, 0
    li s5, 0
    li s6, 0
    li s7, 0
    li s8, 0
    li s9, 0
    li s10, 0
    li s11, 0
    li t3, 0
    li t4, 0
    li t5, 0
    li t6, 0

    li t0, 0x1004
    jr t0 # jump to reset vector
