/*  main.c
 *
 *  Write test to memory-mapped QSPI0
 *  Only work on qemu sifive_u target
 *  On real unleashed board, memory-mapped QSPI0 region is read only.
 */
#include <stdint.h>
#include "rv64_sifive_u.h"
#define MEMMAP_QSPI0 0x20000000
void RAM main(void) {
    register uint32_t *dst;
    dst = (uint32_t *) (MEMMAP_QSPI0 + 0x8000);
    int i;
    for (i = 0; i < 100; i++) {
        *dst = i;
        dst++;
    }
    while(1);
}