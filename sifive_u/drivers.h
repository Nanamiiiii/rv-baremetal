/* drivers.h
 */

/* CLINT Registers (Core Local Interruptor) for time */
#define CLINT_BASE      0x02000000UL
#define CLINT_REG_MTIME (*((volatile uint32_t *)(CLINT_BASE + 0xBFF8)))
#define RTC_FREQ        1000000UL // 1MHz

/* QSPI0 Registers */
#define QSPI0_CTRL       0x10040000UL
#define SPI0_REG_SCKDIV (*((volatile uint32_t *)(QSPI0_CTRL + 0x00)))
#define SPI0_REG_CSMODE (*((volatile uint32_t *)(QSPI0_CTRL + 0x18)))
#define SPI0_REG_FMT    (*((volatile uint32_t *)(QSPI0_CTRL + 0x40)))
#define SPI0_REG_TXDATA (*((volatile uint32_t *)(QSPI0_CTRL + 0x48)))
#define SPI0_REG_RXDATA (*((volatile uint32_t *)(QSPI0_CTRL + 0x4c)))
#define SPI0_REG_TXMARK (*((volatile uint32_t *)(QSPI0_CTRL + 0x50)))
#define SPI0_REG_RXMARK (*((volatile uint32_t *)(QSPI0_CTRL + 0x54)))
#define SPI0_REG_FCTRL  (*((volatile uint32_t *)(QSPI0_CTRL + 0x60)))
#define SPI0_REG_FFMT   (*((volatile uint32_t *)(QSPI0_CTRL + 0x64)))
#define SPI0_REG_IP     (*((volatile uint32_t *)(QSPI0_CTRL + 0x74)))

/* QSPI Fields */
#define SPI_IP_TXWM             0x1
#define SPI_RXDATA_FIFO_EMPTY   (1 << 31)
#define SPI_TXDATA_FIFO_FULL    (1 << 31)
#define SPI_FMT_DIR_TX          (1 << 3)

#define SPI_CSMODE_AUTO         0x0UL
#define SPI_CSMODE_HOLD         0x2UL
#define SPI_CSMODE_MASK         0x3UL

#define SPI_FCTRL_MODE_SEL      0x1UL

#define SPI_FFMT_CMD_EN         0x1
#define SPI_FFMT_ADDR_LEN(x)    (((x) & 0x7) << 1)
#define SPI_FFMT_PAD_CNT(x)     (((x) & 0xf) << 4)
#define SPI_FFMT_CMD_PROTO(x)   (((x) & 0x3) << 8)
#define SPI_FFMT_ADDR_PROTO(x)  (((x) & 0x3) << 10)
#define SPI_FFMT_DATA_PROTO(x)  (((x) & 0x3) << 12)
#define SPI_FFMT_CMD_CODE(x)    (((x) & 0xff) << 16)
#define SPI_FFMT_PAD_CODE(x)    (((x) & 0xff) << 24)

#define SPI_SCKDIV_MASK         0xFFF

#define SPI_TXMARK_MASK         0x3

/* FESPI_REG_FMT Fields */
/* SPI I/O direction */
#define SPI_DIR_RX              0
#define SPI_DIR_TX              1
/* Frame format */
#define SPI_PROTO_S             0 /* Single */
#define SPI_PROTO_D             1 /* Dual */
#define SPI_PROTO_Q             2 /* Quad */

// #define SPI_QUAD_MODE
/* SPI Flash Commands */
#define SPI_READ_ID             0xAB /* Read Flash Identification */
#define SPI_READ_MID            0xAF /* Read Flash Identification, multi-io */
#define SPI_READ_STATUS         0x05 /* Read Status Register */
#define SPI_WRITE_ENABLE        0x06 /* Write Enable */
#define SPI_PAGE_PROGRAM        0x02 /* Page Program (Exists 3-byte address and 4-byte address) */
#define SPI_ROW_PROGRAM         0x62 /* Row Program */
#define SPI_FAST_READ           0x0B /* Fast Read (Exists 3-byte address and 4-byte address) */
#define SPI_READ                0x03 /* Normal Read (Exists 3-byte address and 4-byte address) */
#ifdef SPI_QUAD_MODE
#define SPI_ERASE_SECTOR        0x20 /* Sector Erase (Exists 3-byte address and 4-byte address) */
#else
#define SPI_ERASE_SECTOR        0xD7 /* Sector Erase */
#endif

/* SPI flash status fields (from FESPI_READ_STATUS command) */
#define SPI_RX_BSY              (1 << 0)
#define SPI_RX_WE               (1 << 1)

/* QSPI Flash Sector Size */
#define SPI_FLASH_SECTOR_SIZE   (4 * 1024)


/* PRCI Registers */
#define PRCI_BASE               0x10000000UL
#define PRCI_REG_HFXOSCCFG      (*((volatile uint32_t *)(PRCI_BASE + 0x00)))
#define PRCI_REG_COREPLLCFG     (*((volatile uint32_t *)(PRCI_BASE + 0x04)))
#define PRCI_REG_COREPLLOUT     (*((volatile uint32_t *)(PRCI_BASE + 0x08)))
#define PRCI_REG_DDRPLLCFG_0    (*((volatile uint32_t *)(PRCI_BASE + 0x0c)))
#define PRCI_REG_DDRPLLCFG_1    (*((volatile uint32_t *)(PRCI_BASE + 0x10)))
#define PRCI_REG_GEMGXLPLLCFG_0 (*((volatile uint32_t *)(PRCI_BASE + 0x1c)))
#define PRCI_REG_GEMGXLPLLCFG_1 (*((volatile uint32_t *)(PRCI_BASE + 0x20)))
#define PRCI_REG_CORECLKSEL     (*((volatile uint32_t *)(PRCI_BASE + 0x24)))
#define PRCI_REG_DEVRESET       (*((volatile uint32_t *)(PRCI_BASE + 0x28)))
#define PRCI_REG_CLKMUXSTATUS   (*((volatile uint32_t *)(PRCI_BASE + 0x2c)))
#define PRCI_REG_PROCMONCFG     (*((volatile uint32_t *)(PRCI_BASE + 0xF0)))

#define PLLCFG_R           0x0000003FUL
#define PLLCFG_F           0x000001FFUL
#define PLLCFG_Q           0x00000007UL
#define PLLCFG_RANGE       0x00000007UL
#define PLLCFG_BYPASS      0x00000001UL
#define PLLCFG_FSE         0x00000001UL
#define PLLCFG_LOCK        0x00000001UL
#define PLLCFG_R_SHIFT(r)  ((r & PLLCFG_R)  << 0)
#define PLLCFG_F_SHIFT(f)  ((f & PLLCFG_F)  << 6)
#define PLLCFG_Q_SHIFT(q)  ((q & PLLCFG_Q) << 15)
#define PLLCFG_RANGE_SHIFT(x) ((x & PLLCFG_RANGE) << 18)
#define PLLCFG_BYPASS_SHIFT(x) ((x & PLLCFG_BYPASS) << 24)
#define PLLCFG_FSE_SHIFT(x) ((x & PLLCFG_FSE) << 25)
#define PLLCFG_LOCK_SHIFT(x) ((x & PLLCFG_LOCK) << 31)

#define PLL_CORECLKSEL_HFXIN   0x1
#define PLL_CORECLKSEL_COREPLL 0x0

#define PLLOUT_DIV(x)      (((x) & 0x7F) << 0)
#define PLLOUT_DIV_BY_1(x) (((x) & 0x1)  << 8)
#define PLLOUT_CLK_EN(x)   (((x) & 0x1)  << 31)

#define HFXOSCCFG_EN             (1 << 30)

#define CLKMUX_STATUS_CORECLKPLLSEL          (0x1 << 0)
#define CLKMUX_STATUS_TLCLKSEL               (0x1 << 1)
#define CLKMUX_STATUS_RTCXSEL                (0x1 << 2)
#define CLKMUX_STATUS_DDRCTRLCLKSEL          (0x1 << 3)
#define CLKMUX_STATUS_DDRPHYCLKSEL           (0x1 << 4)
#define CLKMUX_STATUS_GEMGXLCLKSEL           (0x1 << 6)

/* UART */
#define UART0_BASE              0x10010000UL
#define UART_REG_TXDATA         (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UART_REG_RXDATA         (*(volatile uint32_t *)(UART0_BASE + 0x04))
#define UART_REG_TXCTRL         (*(volatile uint32_t *)(UART0_BASE + 0x08))
#define UART_REG_RXCTRL         (*(volatile uint32_t *)(UART0_BASE + 0x0c))
#define UART_REG_IE             (*(volatile uint32_t *)(UART0_BASE + 0x10))
#define UART_REG_IP             (*(volatile uint32_t *)(UART0_BASE + 0x14))
#define UART_REG_DIV            (*(volatile uint32_t *)(UART0_BASE + 0x18))

/* TXDATA Fields */
#define UART_TXEN               (1 <<  0)
#define UART_TXFULL             (1 << 31)

/* RXDATA Fields */
#define UART_RXEN               (1 <<  0)
#define UART_RXEMPTY            (1 << 31)

/* TXCTRL Fields */
#define UART_NSTOP              (1 <<  1)
#define UART_TXCNT(count)       ((0x7 & count) << 16)

/* IP Fields */
#define UART_TXWM               (1 <<  0)


/* Configuration Defaults */

/* Boot (default) Clock settings */
#define PLLREF_FREQ    33330000
#define INIT_FREQ      33000000
#ifndef CPU_FREQ
#define CPU_FREQ       1000000000
#endif
#define MAX_CPU_FREQ   1000000000
#define MAX_FLASH_FREQ  50000000

/* PLL Configuration */
/* R and Q are fixed values for this PLL code */
#define PLL_R (1)  /* First Divisor: By 2 (takes 16Mhz PLLREF / 2 = 8MHz) */
#define PLL_F(cpuHz) (((cpuHz / PLLREF_FREQ) * 2) - 1) /* Multiplier */
#define PLL_Q (1)  /* Second Divisor: By 2 */

#define PLL_R_default 0x1
#define PLL_F_default 0x1F
#define PLL_Q_default 0x3
#define PLL_RANGE_default 0x0
#define PLL_BYPASS_default 0x1
#define PLL_FSE_default 0x1

#define PLLOUT_DIV_default  0x0
#define PLLOUT_DIV_BY_1_default 0x0
#define PLLOUT_CLK_EN_default 0x0

#define PLL_CORECLKSEL_HFXIN   0x1
#define PLL_CORECLKSEL_COREPLL 0x0

/* SPI Serial clock divisor */
#define SPI_SCKDIV_DEFAULT   0x03
#define SPI_SCKDIV_VAL(cpuHz, flashHz) ((cpuHz + 2 * flashHz - 1) / (2 * flashHz))

/* UART baud initialize value */
#ifndef UART_BAUD_INIT
#define UART_BAUD_INIT 115200
#endif
