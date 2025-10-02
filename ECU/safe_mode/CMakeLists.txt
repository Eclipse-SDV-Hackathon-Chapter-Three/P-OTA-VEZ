cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
set(CMAKE_C_STANDARD 99)

# Define o projeto
project(safe_mode C ASM)

# Caminho para o linker script
set(LINKER_SCRIPT /home/andre/SDV/iot-devkit/MXChip/linker_scripts/safe_mode.ld)

include_directories(
    /home/andre/SDV/iot-devkit/MXChip/AZ3166/lib/stm32cubef4/Drivers/STM32F4xx_HAL_Driver/Inc  # HAL
    /home/andre/SDV/iot-devkit/MXChip/AZ3166/lib/stm32cubef4/Drivers/CMSIS/Device/ST/STM32F4xx/Include  # stm32f4xx.h
    /home/andre/SDV/iot-devkit/MXChip/AZ3166/lib/stm32cubef4/Drivers/CMSIS/Include  # core_cm4.h, cmsis_gcc.h
    /home/andre/SDV/iot-devkit/MXChip/AZ3166/app/startup  # startup headers
    /home/andre/SDV/iot-devkit/MXChip/AZ3166/lib/stm32cubef4/config  # stm32f4xx_hal_conf.h
    /home/andre/SDV/iot-devkit/shared/lib/threadx/common/inc  # ThreadX
    /home/andre/SDV/iot-devkit/shared/lib/threadx/ports/cortex_m4/gnu/inc
    /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/arm-none-eabi/include  # Para stdint.h e stddef.h do toolchain
    /home/andre/SDV/iot-devkit/shared/src

)

file(GLOB HAL_SRC
    /home/andre/SDV/iot-devkit/MXChip/AZ3166/lib/stm32cubef4/Drivers/STM32F4xx_HAL_Driver/Src/*.c
)

add_executable(bootloader
    main.c
    /home/andre/SDV/iot-devkit/MXChip/AZ3166/app/startup/startup_stm32f412rx.s
    /home/andre/SDV/iot-devkit/MXChip/AZ3166/app/startup/system_stm32f4xx.c
    /home/andre/SDV/iot-devkit/MXChip/AZ3166/app/startup/tx_initialize_low_level.S
    /home/andre/SDV/iot-devkit/shared/src/uart.c
    ${HAL_SRC}  # ADICIONA ISTO
)

# Definições de compilação
target_compile_definitions(safe_mode PRIVATE
    STM32F412Rx  # CORRIGIDO: usa Rx para incluir stm32f412rx.h
    USE_HAL_DRIVER
    __IO=volatile
)

# Liga bibliotecas
target_link_libraries(safe_mode PRIVATE
    threadx
    /home/andre/SDV/iot-devkit/MXChip/AZ3166/build/lib/stm32hal/libstm32hal.a
)

# Opções de linker
target_link_options(safe_mode PRIVATE
    -T${LINKER_SCRIPT}
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -Wl,--gc-sections
)

# Dependências para o linker script
set_target_properties(safe_mode PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
