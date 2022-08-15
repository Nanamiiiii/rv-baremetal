/* drivers.c
 *
 * drivers for FU540 SoC
 * - PLL
 * - UART
 * - QSPI0
 */

#include <stdint.h>
#include <string.h>
#include "rv64_sifive_u.h"
#include "drivers.h"

/* UART functions for HiFive Unleashed UART */
void uart_write(char c) {
    while ((UART_REG_TXDATA & UART_TXFULL) != 0);
    UART_REG_TXDATA = (uint32_t)c;
}

char uart_read(void) {
    uint32_t ch;
    do {
        ch = UART_REG_RXDATA;
    } while ((ch & UART_RXEMPTY) != 0);
    return (char)(ch & 0xFF);
}

void uart_init(uint32_t cpu_clock, uint32_t baud_rate) {
    uint32_t div_val = (cpu_clock / baud_rate) - 1;
    UART_REG_DIV = div_val;
    UART_REG_TXCTRL |= UART_TXEN;
    UART_REG_RXCTRL |= UART_RXEN;
}

void uart_flush(void) {
    uint32_t bits_per_symbol, cycles_to_wait;
    volatile uint32_t x;

    UART_REG_TXCTRL &= ~(UART_TXCNT(0x7));
    UART_REG_TXCTRL |= UART_TXCNT(1);
    while((UART_REG_IP & UART_TXWM) == 0);

    bits_per_symbol = (UART_REG_TXCTRL & (1 << 1)) ? 9 : 10;
    cycles_to_wait = bits_per_symbol * (UART_REG_DIV + 1);
    for(x = 0; x < cycles_to_wait; x++) {
        asm volatile ("nop");
    }
}

/* QSPI functions */
void spi_init(uint32_t cpu_clock, uint32_t flash_freq) {
    uint32_t quotient;
    SPI0_REG_SCKDIV &= ~SPI_SCKDIV_MASK;
    quotient = SPI_SCKDIV_VAL(cpu_clock, flash_freq);
    if (quotient == 0) {
        SPI0_REG_SCKDIV |= 0x0;
    } else {
        SPI0_REG_SCKDIV |= quotient - 1;
    }
}

static RAM void spi_swmode(void) {
    asm volatile("fence");
    asm volatile("fence.i");
    if (SPI0_REG_FCTRL & SPI_FCTRL_MODE_SEL)
        SPI0_REG_FCTRL &= ~SPI_FCTRL_MODE_SEL;
}

static RAM void spi_hwmode(void) {
    uint32_t x;
    if ((SPI0_REG_FCTRL & SPI_FCTRL_MODE_SEL) == 0)
        SPI0_REG_FCTRL |= SPI_FCTRL_MODE_SEL;
    asm volatile("fence");
    asm volatile("fence.i");
    for(x = 0; x < CPU_FREQ / 500; x++) {
        asm volatile ("nop");
    }
}

static RAM void spi_csmode_hold(void) {
    uint32_t reg = SPI0_REG_CSMODE & ~SPI_CSMODE_MASK;
    SPI0_REG_CSMODE = reg | SPI_CSMODE_HOLD;
}

static RAM void spi_csmode_auto(void) {
    uint32_t reg = SPI0_REG_CSMODE & ~SPI_CSMODE_MASK;
    SPI0_REG_CSMODE = reg | SPI_CSMODE_AUTO;
}

static RAM void spi_wait_txwm(void) {
    while((SPI0_REG_IP & SPI_IP_TXWM) == 0)
        ;
}

static RAM void spi_sw_tx(uint8_t b) {
    while((SPI0_REG_TXDATA & SPI_TXDATA_FIFO_FULL) != 0)
        ;
    SPI0_REG_TXDATA = b;
}

static RAM uint8_t spi_sw_rx(void) {
    volatile uint32_t reg;
    do {
        reg = SPI0_REG_RXDATA;
    } while ((reg & SPI_RXDATA_FIFO_EMPTY) != 0);
    return (uint8_t)(reg & 0xFF);
}

static RAM void spi_sw_setdir(int tx) {
    if (tx)
        SPI0_REG_FMT |= SPI_FMT_DIR_TX;
    else
        SPI0_REG_FMT &= ~SPI_FMT_DIR_TX;
}

static RAM void spi_write_address(uint32_t address) {
    spi_sw_tx((address & 0xFF0000) >> 16);
    spi_sw_tx((address & 0xFF00) >> 8);
    spi_sw_tx((address & 0xFF));
    spi_wait_txwm();
}

static RAM void spi_write_4address(uint32_t address) {
    spi_sw_tx((address & 0xFF000000) >> 24);
    spi_sw_tx((address & 0xFF0000) >> 16);
    spi_sw_tx((address & 0xFF00) >> 8);
    spi_sw_tx((address & 0xFF));
    spi_wait_txwm();
}

static RAM void spi_wait_write_disabled(void) {
    uint8_t rx;
    spi_sw_setdir(SPI_DIR_RX);
    spi_csmode_hold();
    spi_sw_tx(SPI_READ_STATUS);
    rx = spi_sw_rx();
    while (1) {
        spi_sw_tx(0);
        rx = spi_sw_rx();
        if ((rx & SPI_RX_WE) == 0) {
            break;
        }
    }
    spi_csmode_auto();
    spi_sw_setdir(SPI_DIR_TX);
}

static RAM void spi_write_enable(void)
{
    uint8_t rx;
    int i;
    while(1) {
        spi_sw_tx(SPI_WRITE_ENABLE);
        spi_wait_txwm();
        spi_sw_setdir(SPI_DIR_RX);
        spi_csmode_hold();
        spi_sw_tx(SPI_READ_STATUS);
        rx = spi_sw_rx();
        for (i = 0; i < 3; i++) {
            spi_sw_tx(0);
            rx = spi_sw_rx();
            if ((rx & SPI_RX_WE) == SPI_RX_WE) {
                spi_csmode_auto();
                spi_sw_setdir(SPI_DIR_TX);
                return;
            }
        }
        spi_csmode_auto();
        spi_sw_setdir(SPI_DIR_TX);
    }
}

static RAM void spi_wait_flash_busy(void)
{
    uint8_t rx;
    spi_sw_setdir(SPI_DIR_RX);
    spi_csmode_hold();
    spi_sw_tx(SPI_READ_STATUS);
    while (1) {
        spi_sw_tx(0);
        rx = spi_sw_rx();
        if ((rx & SPI_RX_BSY) == 0) {
            break;
        }
    }
    spi_csmode_auto();
    spi_sw_setdir(SPI_DIR_TX);
}

static uint32_t RAM spi_flash_probe(void) {
    uint32_t rx;

    SPI0_REG_TXMARK = 1;
    spi_sw_setdir(SPI_DIR_RX);
    spi_swmode();

    spi_wait_txwm();
    spi_wait_flash_busy();
    spi_sw_setdir(SPI_DIR_RX);
    spi_csmode_hold();
    spi_sw_tx(SPI_READ_ID);
    spi_sw_tx(0);
    spi_sw_tx(0);
    spi_sw_tx(0);
    rx = spi_sw_rx();
    rx |= spi_sw_rx() << 8;
    rx |= spi_sw_rx() << 16;
    spi_csmode_auto();
    spi_sw_setdir(SPI_DIR_TX);
    return rx;
}

/* PRCI initialization */
void prci_init() {
    uint32_t config_val = 0;
    uint32_t core_out = 0;

    uint32_t core_clock = MAX_CPU_FREQ;     // 1GHz
    uint32_t uart_baud = UART_BAUD_INIT;    // 115200

    /* init UART divider */
    uint64_t peripheral_input_freq;
    if (PRCI_REG_CLKMUXSTATUS & CLKMUX_STATUS_TLCLKSEL) {
        peripheral_input_freq = INIT_FREQ;
    } else {
        peripheral_input_freq = INIT_FREQ / 2;
    }
    uart_init(peripheral_input_freq, uart_baud);

    /* PLL Configuration */
    uint32_t pll_default =
        (PLLCFG_R_SHIFT(PLL_R_default)) |
        (PLLCFG_F_SHIFT(PLL_F_default)) |
        (PLLCFG_Q_SHIFT(PLL_Q_default)) |
        (PLLCFG_RANGE_SHIFT(PLL_RANGE_default)) |
        (PLLCFG_BYPASS_SHIFT(PLL_BYPASS_default)) |
        (PLLCFG_FSE_SHIFT(PLL_FSE_default));

    uint64_t lockmask = ~PLLCFG_LOCK_SHIFT(1);

    uint32_t pllout_default =
        (PLLOUT_DIV(PLLOUT_DIV_default)) |
        (PLLOUT_DIV_BY_1(PLLOUT_DIV_BY_1_default)) |
        (PLLOUT_CLK_EN(PLLOUT_CLK_EN_default));

    if ((PRCI_REG_COREPLLCFG ^ pll_default) & lockmask) return;
    if ((PRCI_REG_COREPLLOUT ^ pllout_default)) return;

    /* CORE PLL initialization */
    if (PRCI_REG_CLKMUXSTATUS & CLKMUX_STATUS_TLCLKSEL) {
        /* 500MHz */
        peripheral_input_freq = 500000000;
        uart_init(peripheral_input_freq, UART_BAUD_INIT);
        spi_init(peripheral_input_freq, MAX_FLASH_FREQ);
        config_val = 
            (PLLCFG_R_SHIFT(0)) |
            (PLLCFG_F_SHIFT(59)) |
            (PLLCFG_Q_SHIFT(3)) |
            (PLLCFG_RANGE_SHIFT(0x4)) |
            (PLLCFG_BYPASS_SHIFT(0)) |
            (PLLCFG_FSE_SHIFT(1)); 
        PRCI_REG_COREPLLCFG = config_val;

        /* wait for lock */
        while ((PRCI_REG_COREPLLCFG & (PLLCFG_LOCK_SHIFT(1))) == 0);

        core_out =
            (PLLOUT_DIV(PLLOUT_DIV_default)) |
            (PLLOUT_DIV_BY_1(PLLOUT_DIV_BY_1_default)) |
            (PLLOUT_CLK_EN(1));        
        PRCI_REG_COREPLLOUT = core_out;

        PRCI_REG_CORECLKSEL = PLL_CORECLKSEL_COREPLL;
    } else {
        /* 1GHz */
        peripheral_input_freq = 1000000000 / 2;
        uart_init(peripheral_input_freq, UART_BAUD_INIT);
        spi_init(peripheral_input_freq, MAX_FLASH_FREQ);
        config_val = 
            (PLLCFG_R_SHIFT(0)) |
            (PLLCFG_F_SHIFT(59)) |
            (PLLCFG_Q_SHIFT(2)) |
            (PLLCFG_RANGE_SHIFT(0x4)) |
            (PLLCFG_BYPASS_SHIFT(0)) |
            (PLLCFG_FSE_SHIFT(1)); 
        PRCI_REG_COREPLLCFG = config_val;

        /* wait for lock */
        while ((PRCI_REG_COREPLLCFG & (PLLCFG_LOCK_SHIFT(1))) == 0);

        core_out =
            (PLLOUT_DIV(PLLOUT_DIV_default)) |
            (PLLOUT_DIV_BY_1(PLLOUT_DIV_BY_1_default)) |
            (PLLOUT_CLK_EN(1));        
        PRCI_REG_COREPLLOUT = core_out;

        PRCI_REG_CORECLKSEL = PLL_CORECLKSEL_COREPLL;
    }
}

void ddr_init() {
    /* unimplemented */
}

void phy_init() {
    /* unimplemented */
}
