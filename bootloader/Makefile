CC=arm-none-eabi-gcc
CFLAGS=-mcpu=cortex-m4 -mthumb -Os -ffreestanding -nostdlib \
    -I$(PWD)/stm32cubef4/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
    -I$(PWD)/stm32cubef4/Drivers/CMSIS/Include \
    -DSTM32F412Rx

LDFLAGS=-Tld/linker_boot.ld -Wl,--gc-sections

SYSTEM=$(PWD)/stm32cubef4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c

OBJS=startup_stm32f412xx.o src/main.o system_stm32f4xx.o

# Default target
all: bootloader.bin

# Assembly files
%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

# C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Cube system file
system_stm32f4xx.o: $(SYSTEM)
	$(CC) $(CFLAGS) -c $< -o $@

# Link everything
bootloader.elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Convert ELF to BIN
bootloader.bin: bootloader.elf
	arm-none-eabi-objcopy -O binary $< $@

# Clean
clean:
	rm -f *.o src/*.o bootloader.elf bootloader.bin
