CROSS_COMPILE	:= riscv64-unknown-elf-
CC				:= $(CROSS_COMPILE)gcc
LD				:= $(CROSS_COMPILE)gcc
AS				:= $(CROSS_COMPILE)gcc
OBJCOPY			:= $(CROSS_COMPILE)objcopy
CFLAGS			:= -Os -mcmodel=medany -march=rv64imac -mabi=lp64 -g -ggdb -Wall -Wno-main -Wstack-usage=3896 -ffreestanding -Wno-unused -nostdlib -nostartfiles -fno-builtin-printf -ffunction-sections -fdata-sections
LSCRIPT			:= target.ld
LDFLAGS         := -T $(LSCRIPT) -Wl,--gc-sections -Wl,-Map=image.map -nostdlib -mcmodel=medany -march=rv64imac -mabi=lp64
ASFLAGS			:= $(CFLAGS)

OBJS			:= bootup.o vector.o main.o

.PHONY: all
all: image.bin

image.bin: image.elf
	$(OBJCOPY) -O binary $^ $@

image.elf: $(OBJS) $(LSCRIPT)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.s
	$(AS) $(ASFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm -f image.bin image.elf $(OBJS) image.map
